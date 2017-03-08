import os
import subprocess,shlex
import re
from collections import defaultdict
import random

def extractCategories(prefix,lImg):
    prelen=len(prefix)
    image_categories=[re.sub('[^a-z]','',img[prelen:].split(".")[0]) for img in lImg]
    return(image_categories)

def calcPrecision(actual,lImg,maps):
    predicted=sum([1 for i in lImg[:11] if i==actual])
    total=len(maps[actual])-1
    return(predicted/total)

IMAGES_DIR="./part1_images/"
all_files=os.listdir(IMAGES_DIR)

image_categories=extractCategories("",all_files)
image_cat_map=defaultdict(list)

for i in range(len(all_files)):
    image_cat_map[image_categories[i]].append(all_files[i])

print('Precision for Categories')
for cat in image_cat_map:
    query=random.choice(image_cat_map[cat])
    query_searchspace=[IMAGES_DIR+i for i in all_files if i != query]
    query=IMAGES_DIR+query

    #Run the Stat
    args=['./a2','part1',query]+query_searchspace
    output=subprocess.Popen(" ".join(args),shell=True, stdout=subprocess.PIPE)

    #get and process the result
    print(cat,calcPrecision(cat,extractCategories(IMAGES_DIR,[i for i in output.communicate()[0].decode('utf-8').split("\n") if i!=""]),image_cat_map))



    # print("Precision:{0}".format(calcPrecision(cat,predicted)))
