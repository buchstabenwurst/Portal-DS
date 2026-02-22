file=$(kdialog --getopenfilename ./Textures/ '*.png *.bmp')
name=$(basename $file | cut -d"." -f1)
echo $file
echo $name
python /home/joshua/Programming/ds/tools/img2ds/img2ds.py --input $file --name $name --output "../data" --format "PAL4"
