import hid
import os
from configparser import ConfigParser
from time import sleep



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
# 0x04 : oled set brightness
#   0x0x : brightness value
# 0x05 : query device
#   0x01 : is_oled_on
#   0x02 : oled_on
#   0x03 : oled_off
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
        self.cols = int(config['DEVICE']['COLUMNS'])
        self.rows = int(config['DEVICE']['ROWS'])

        self.cmd_line   = b'\x01'

        self.cmd_pixel  = b'\x02'
        self.end_pixel  = b'\xff'

        self.cmd_scroll = b'\x03'
        self.scroll_on  = b'\x02'
        self.scroll_off = b'\x01'
        self.scroll_lft = b'\x04'
        self.scroll_rgt = b'\x03'
        self.scrl_speed = b'\x05'
        self.scrl_area  = b'\x06'

        self.cmd_bright = b'\x04'

        self.oled       = b'\x05'
        self.oled_state = b'\x01'
        self.oled_on    = b'\x02'
        self.oled_off   = b'\x03'

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
        self.device.write(data)

    def read(self, size):
        '''reads from device'''
        # self.read_buffer = zeros(self.raw_epsize)
        return self.device.read(size)

    def close(self):
        '''always close your connection'''
        self.device.close()

    def pad(self, data):
        '''pad the data just before it's send'''
        return data + b'\x00' * (self.raw_epsize - len(data))

    def tobyte(self, data):
        if type(data) is bytes:
            return data
        return (data).to_bytes(1, 'big')

    def tobytes(self, data):
        ''' the output here can be directly sent to self.write'''
        out = b''
        for num in data:
            if type(num) is str:
                num = str.encode(num)
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

    def send_raw(self, data):
        '''send raw data
        DO NOT DO THIS unless you know what you are sending'''
        self.device.write(data)

    def stop_scroll(self):
        '''stops a scroll'''
        data = [self.cmd_scroll, self.scroll_off]
        self.write(data)

    def scroll_right(self):
        '''scrolls buffer right'''
        data = [self.cmd_scroll, self.scroll_rgt]
        self.write(data)

    def scroll_left(self):
        '''scrolls buffer left'''
        data = [self.cmd_scroll, self.scroll_lft]
        self.write(data)

    def scroll_speed(self, speed):
        '''sets scroll speed'''
        data = [self.cmd_scroll, self.scrl_speed, speed]
        self.write(data)

    def scroll_area(self, start, end):
        '''defines scroll area'''
        data = [self.cmd_scroll, self.scrl_area, start, end]
        self.write(data)

    def set_brightness(self, brightness):
        '''sets the brightness'''
        if not 0 <= brightness <= 255:
            raise Exception("Invalid value: {}".format(brightness))
        data = [self.cmd_bright, brightness]
        self.write(data)

    def get_scroll_state(self):
        pass

    def vibrate(self, speed):
        pass

    def scroll_text(self, line, text, delay=.1):
        '''scrolls any text Note: i think if it's less than
        the width of the screen, it will look funny...'''
        def gen_data(text):
            data = list(range(self.cols))
            for index, letter in enumerate(text):
                try:
                    data[index] = letter
                except:
                    data = "".join(data)
                    return(data)
        data = gen_data(text)
        self.send_line(line, data)
        sleep(delay)
        for rotate in range(len(text)):
            text = list(text)
            text.append(text.pop(0))
            text = "".join(text)
            data = gen_data(text)
            self.send_line(line, data)
            sleep(delay)


    def get_oled_state(self):
        data = [self.oled, self.oled_state]
        self.write(data)
        oled_state = self.read(size=self.raw_epsize)
        oled_state = int.from_bytes(oled_state, 'little')
        if oled_state:
            return(True)
        else:
            return(False)

    def turn_oled_on(self):
        ''' turns on oled and waits .5 seconds before doing anything'''
        data = [self.oled, self.oled_on]
        self.write(data)
        sleep(.5)

    def turn_oled_off(self):
        ''' turns off oled and waits .2 seconds before doing anything'''
        data = [self.oled, self.oled_off]
        self.write(data)
        sleep(.2)



def main():
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    config_path = os.path.join(dirname, 'config.ini')

    try:
        me = QMKDevice(config_path)
        me.clear_screen()
        me.send_line(line=0, data="Hello World")
        sleep(1)
        me.send_line(line=2, data="Hello World")
        sleep(1)
        for x in range(8):
            me.send_line(line=x, data="Hello World")
        sleep(1)
        me.send_line(line=1, data="line number 2")
        sleep(1)
        for x in range(8):
            me.clear_line(line=x)
            sleep(1)
        me.scroll_text(line=0, text="Here is an example of some scrolling text...")

        for x in range(10):
            me.turn_oled_off()
            me.turn_oled_on()
        me.get_oled_state()
        for x in range(0, 255, 25):
            me.set_brightness(x)
            sleep(.1)
        for x in range(255, 0, -25):
            me.set_brightness(x)
            sleep(.1)
        me.set_brightness(255)



    finally:
        me.close()

if __name__ == '__main__':
    main()
