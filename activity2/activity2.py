import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import sys

def readFile(name,xdim=5000,ydim=5000):
"""
read the file and create a matrix representation of the image.
"""
    image=np.zeros((xdim,ydim),dtype=np.float32)
    with open(name) as fl:
        for ln in fl.readlines():
            line=ln.split(" ")
            image[int(line[0])][int(line[1])]=1
    return(image)

### Main : Get the input file name, write the image.
plt.imsave("output.png",readFile(sys.argv[1]))
