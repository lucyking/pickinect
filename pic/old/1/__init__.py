# -*- coding: utf-8 -*-
import re
import os
from PIL import Image
import images2gif
import shutil


class Alpha(object):
    ROBOT_LISTENER_API_VERSION = 2


    def gengif(self):


        myPicList = self.GetDirImageList(os.path.abspath('.'), False)
        # human sort png list eg: [1,11,2] -- > [1,2,11]
        myPicList = self.sort_nicely(myPicList)

        if myPicList:
            self.GetGifAnimationFromImages('result.gif', myPicList, 2)


    def GetGifAnimationFromImages(self, targetGifFilePath, srcImageFilePaths, type=0):
        images = []
        maxWidthAndHeight = 1
        maxWidth = 1
        maxHeight = 1
        widthAndFilePaths = []
        heightAndFilePaths = []

        for imageFilePath in srcImageFilePaths:
            fp = open(imageFilePath, "rb")
            width, height = Image.open(fp).size
            widthAndFilePaths.append((width, imageFilePath))
            heightAndFilePaths.append((height, imageFilePath))
            maxWidth = max(maxWidth, width)
            maxHeight = max(maxHeight, height)
            fp.close()

        maxWidthAndHeight = max(maxWidthAndHeight, maxWidth, maxHeight)

        # 降序排列
        widthAndFilePaths.sort(key=lambda item: item[0], reverse=True)
        heightAndFilePaths.sort(key=lambda item: item[0], reverse=True)

        if type == 4 or type == 5:
            if type == 4:
                for widthAndFilePath in widthAndFilePaths:
                    img = Image.open(widthAndFilePath[1])
                    images.append(img)
            if type == 5:
                for heightAndFilePath in heightAndFilePaths:
                    img = Image.open(heightAndFilePath[1])
                    images.append(img)
        else:
            for imageFilePath in srcImageFilePaths:
                fp = open(imageFilePath, "rb")
                img = Image.open(fp)
                width, height = img.size
                if type == 0 or type == 2:
                    imgResizeAndCenter = Image.new("RGB", [maxWidth, maxHeight], (255, 255, 255))
                elif type == 1 or type == 3:
                    imgResizeAndCenter = Image.new("RGB", [maxWidthAndHeight, maxWidthAndHeight], (255, 255, 255))

                if type == 0:
                    if maxWidth / width >= maxHeight / height:
                        resizeImg = img.resize((width * maxHeight / height, maxHeight), Image.ANTIALIAS)
                        imgResizeAndCenter.paste(resizeImg, ((maxWidth - width * maxHeight / height) / 2, 0))
                    else:
                        resizeImg = img.resize((maxWidth, height * maxWidth / width), Image.ANTIALIAS)
                        imgResizeAndCenter.paste(resizeImg, (0, (maxHeight - height * maxWidth / width) / 2))
                if type == 1:
                    if width >= height:
                        resizeImg = img.resize((maxWidthAndHeight, height * maxWidthAndHeight / width), Image.ANTIALIAS)
                        imgResizeAndCenter.paste(resizeImg,
                                                 (0, (maxWidthAndHeight - height * maxWidthAndHeight / width) / 2))
                    else:
                        resizeImg = img.resize((width * maxWidthAndHeight / height, maxWidthAndHeight), Image.ANTIALIAS)
                        imgResizeAndCenter.paste(resizeImg,
                                                 ((maxWidthAndHeight - width * maxWidthAndHeight / height) / 2, 0))
                elif type == 2:
                    imgResizeAndCenter.paste(img, ((maxWidth - width) / 2, (maxHeight - height) / 2))
                elif type == 3:
                    imgResizeAndCenter.paste(img, ((maxWidthAndHeight - width) / 2, (maxWidthAndHeight - height) / 2))

                images.append(imgResizeAndCenter)
                fp.close()

        images2gif.writeGif(targetGifFilePath, images, duration=1, nq=0.1)

    def GetDirImageList(self, dir_proc, recusive=True):
        resultList = []
        for file in os.listdir(dir_proc):
            if re.search(r'.*png', file):  # <------ select specified picture type
                if os.path.isdir(os.path.join(dir_proc, file)):
                    if (recusive):
                        resultList.append(self.GetDirImageList(os.path.join(dir_proc, file), recusive))
                    continue
                resultList.append(os.path.join(dir_proc, file))
        return resultList

    # human sort
    def tryint(self, s):
        try:
            return int(s)
        except:
            return s

    def alphanum_key(self, s):
        return [self.tryint(c) for c in re.split('([0-9]+)', s)]

    def sort_nicely(self, l):
        l.sort(key=self.alphanum_key)
        return l



if __name__ == '__main__':
    a = Alpha()
    a.gengif()
