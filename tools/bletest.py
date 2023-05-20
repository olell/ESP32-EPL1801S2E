import asyncio
from bleak import BleakClient, BleakScanner
import time
import sys
from PIL import Image


def convert_image_to_monochrome(image_path, desired_width=384):
    # Open the image file
    image = Image.open(image_path)

    # Convert the image to monochrome
    image = image.convert("1")

    # Scale the image width to the desired width while maintaining the aspect ratio
    width, height = image.size
    desired_height = int(height * (desired_width / width))
    image = image.resize((desired_width, desired_height), Image.ANTIALIAS)

    # Convert the image to a list of rows, where each row is represented by 48 bytes
    rows = []
    row_bytes = bytearray(48)  # Initialize a 48-byte row
    byte_index = 0  # Current byte index within the row
    bit_index = 0  # Current bit index within the byte

    for y in range(0, desired_height):
        for x in range(0, desired_width):
            # Each bit represents one pixel: 0 for black, 1 for white
            pixel = 1 if image.getpixel((x, y)) < 128 else 0

            # Set the corresponding bit within the current byte
            row_bytes[byte_index] |= pixel << bit_index

            bit_index += 1
            if bit_index >= 8:
                bit_index = 0
                byte_index += 1

        # Append the row to the rows list if it's complete (48 bytes)
        if byte_index >= 48:
            rows.append(bytes(row_bytes))
            row_bytes = bytearray(48)
            byte_index = 0

    return rows


rows = convert_image_to_monochrome(sys.argv[1])

device = "10955197-82B0-18EE-5A5A-C4667246EF86"

SERVICE_UUID = "ADE5F08B-C00E-418F-942B-297339F7B564"
WRITE_CHARACTERISTIC_UUID = "8b26fafb-406b-425b-b576-8ba4c37d77f2".upper()


async def scan():
    devices = await BleakScanner.discover()
    for d in devices:
        print(d)


HEADER = b"\x70\x0a" + (b"\x00" * 30)


async def connect():
    async with BleakClient(device) as client:
        service = client.services.get_service(SERVICE_UUID)
        write_char = service.get_characteristic(WRITE_CHARACTERISTIC_UUID)

        pc = 0
        for i in range(0, len(rows), 200):
            for j in range(0, 20):
                output_data = b""
                l = 0
                for k in range(0, 10):
                    ridx = i + (j * 10) + k
                    if ridx < len(rows):
                        output_data += rows[ridx]
                        l += 1
                output_data = bytes([0x70, l]) + b"\x00" * 30 + output_data
                await client.write_gatt_char(write_char, output_data, response=True)
                pc += 1
                print("Send packet:", pc)
                time.sleep(0.01)


asyncio.run(connect())
