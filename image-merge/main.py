import sys
from PIL import Image
Image.MAX_IMAGE_PIXELS = 294912000

if len(sys.argv) < 3 or sys.argv[1] == None or sys.argv[2] == None:
    print("Need to specify two images.")
    quit()

im1 = Image.open(sys.argv[1])
im2 = Image.open(sys.argv[2])

def get_concat_h(im1, im2):
    dst = Image.new('RGB', (im1.width + im2.width, im1.height))
    dst.paste(im1, (0, 0))
    dst.paste(im2, (im1.width, 0))
    return dst

def get_concat_v(im1, im2):
    dst = Image.new('RGB', (im1.width, im1.height + im2.height))
    dst.paste(im1, (0, 0))
    dst.paste(im2, (0, im1.height))
    return dst

get_concat_h(im1, im2).save('output.png')
# get_concat_v(im1, im2).save('data/dst/pillow_concat_v.jpg')