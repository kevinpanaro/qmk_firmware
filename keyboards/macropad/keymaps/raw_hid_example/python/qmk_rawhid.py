import hid
import os
from configparser import ConfigParser
from time import sleep


#
# data
# 0x01 : write string to a row
#   0x0X : X is the row rest of data is byte string to write to that row
# 0x02 :
#   0x<column>0x<row> : turn this pixel on.
# 0x03 : scroll commands
#   0x00: turn off scroll
#   0x01: turn on scroll
#   0x02: scroll right
#   0x03: scroll left
#   0x04: scroll speed
#     0x0X: where x is a value of 0 to 7
#   0x05: scroll area
#     0x<start>0x<stop> : start_line and end_line
# 0x08 : clearing a row
#   0x0X : where X is the row to clear
#   0x08 : clear all rows


class QMKDevice():
    def __init__(self, config_path):
        config = ConfigParser()
        config.read(config_path)

        def _to_int(string, base=16):
            return int(string, base)

        self.vid = _to_int(config['DEVICE']['VID'])
        self.pid = _to_int(config['DEVICE']['PID'])
        self.usage_page = _to_int(config['DEVICE']['USAGE_PAGE'])
        self.usage = _to_int(config['DEVICE']['USAGE'])
        self.raw_epsize = int(config['DEVICE']['RAW_EPSIZE'])

        self.cmd_line   = b'\x01'

        self.cmd_pixel  = b'\x02'
        self.end_pixel  = b'\xff'

        self.cmd_scroll = b'\x03'
        self.scroll_on  = b'\x01'
        self.scroll_off = b'\x00'
        self.scroll_lft = b'\x03'
        self.scroll_rgt = b'\x02'
        self.scrl_speed = b'\x04'
        self.scrl_area  = b'\x05'

        self.cmd_erase  = b'\x08'

        devices = hid.enumerate()
        for device in devices:
            if (
                device["vendor_id"] == self.vid
                and device["product_id"] == self.pid
                and device["usage_page"] == self.usage_page
                and device["usage"] == self.usage
            ):
                self.device = hid.Device(path=device['path'])
                break



    def get_device_info(self):
        print("Device manufacturer: {}".format(self.device.manufacturer))
        print("Product: {}".format(self.device.product))

    def write(self, data):
        '''write to device'''
        data = self.tobytes(data)
        # self.scroll_off()
        self.device.write(data)

    def close(self):
        '''always close your connection'''
        self.device.close()

    def pad(self, data):
        '''pad the data just before it's send'''
        return data + b'\x00' * (self.raw_epsize - len(data))

    def tobyte(self, data):
        if type(data) is bytes:
            return data
        else:
            return (data).to_bytes(1, 'big')

    def tobytes(self, data):
        ''' the output here can be directly sent to self.write'''
        out = b''
        for num in data:
            out += self.tobyte(num)
        out = self.pad(out)
        return out

    def clear_screen(self):
        '''clears the screen'''
        data = [self.cmd_erase, self.cmd_erase]
        self.write(data)

    def clear_line(self, line):
        '''clears a given line'''
        data = [self.cmd_erase, line]
        self.write(data)

    def send_pixels(self, pixels, offset=(0,0)):
        '''turns on one pixel'''
        data = []
        for pixel in pixels:
            data += list(sum(x) for x in zip(pixel, offset))
        data.insert(0, self.cmd_pixel)
        data.append(self.end_pixel)
        self.write(data)

    def send_line(self, line, data):
        '''sends data to a line'''
        self.clear_line(line)
        data = [self.cmd_line, line, data]
        self.write(data)

    def scroll_on(self):
        data = [self.cmd_scroll, self.scroll_on]
        self.write(data)

    def scroll_off(self):
        data = [self.cmd_scroll, self.scroll_off]
        self.write(data)

    def scroll_right(self):
        data = [self.cmd_scroll, self.scroll_rgt]
        self.write(data)

    def scroll_left(self):
        data = [self.cmd_scroll, self.scroll_lft]
        self.write(data)

    def scroll_speed(self, speed):
        data = [self.cmd_scroll, self.scrl_speed, speed]
        self.write(data)

    def scroll_area(self, start, end):
        data = [self.cmd_scroll, self.scrl_area, start, end]
        self.write(data)



def main():
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    config_path = os.path.join(dirname, 'config.ini')

    try:
        me = QMKDevice(config_path)
        me.clear_screen()
        me.send_line(line=3, data=b'Hello!')
        # me.send_line(line=7, data=b'Artist :  Michael Nau')
        # me.scroll_area(start=0, end=7)
        # me.send_line(line=4, data=b' scroll')
        # me.scroll_right()



    finally:
        me.close()

if __name__ == '__main__':
    main()
