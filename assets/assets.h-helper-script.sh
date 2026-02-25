#!/bin/bash
# usage ./assets.h-helper-script.sh textureName gritFile assets/Textures source
TextureName=$1
TextureBaseFolder=$3
Sorce=$4
IngameName=$(echo $1 | cut -c $(echo -n ./$TextureBaseFolder | wc -m)-)
FriendlyIngameName=$(echo $1 | cut -c $(echo -n ./$TextureBaseFolder | wc -m)- | tr / _)
Basename=$(basename -s .png $1.png)

TextureFormatGrit=$(grep -v '^#' ./$2)
NumColors=$(echo ${TextureFormatGrit#*-pn} | cut -f 1 -d " ")
TextureFormatGrit=$(echo ${TextureFormatGrit#*-gB} | cut -f 1 -d " ")
re='^[0-9]+$'
#TODO fix "if NumColors is a number"
case $TextureFormatGrit in
  1)
    TextureFormat=NE_PAL4
    if ! [[ $NumColors =~ $re ]] ; then
      NumColors=2
    fi
    ;;

  2)
    TextureFormat=NE_PAL4
    if ! [[ $NumColors =~ $re ]] ; then
      NumColors=4
    fi
    ;;

  4)
    TextureFormat=NE_PAL16
    if ! [[ $NumColors =~ $re ]] ; then
      NumColors=16
    fi
    ;;

  8)
    TextureFormat=NE_PAL16
    if ! [[ $NumColors =~ $re ]] ; then
      NumColors=256
    fi
    ;;

  16)
    TextureFormat=NE_A1RGB5
    NumColors=-1
    ;;

  a5i3)
    TextureFormat=NE_A5PAL8
    if ! [[ $NumColors =~ $re ]] ; then
      NumColors=8
    fi
    ;;

  a3i5)
    TextureFormat=NE_A3PAL32
    if ! [[ $NumColors =~ $re ]] ; then
      NumColors=32
    fi
    ;;
esac

sed -i "$(wc -l < $Sorce/assetsTemplate.h)i\\#include \"$IngameName.h\"\\" $Sorce/assets.h
# sed -i "7i\\MaterialMetadata Material_$FriendlyIngameName;\\" $Sorce/assetsArray.h
echo -n "NE_Material* NEMaterial_$FriendlyIngameName;" >> $Sorce/assetsTMP.h
echo -n "NE_Palette* NEPallette_$FriendlyIngameName;" >> $Sorce/assetsTMP.h
echo -n const MaterialMetadata Material_$FriendlyIngameName = { >> $Sorce/assetsTMP.h
echo -n \"$(echo $IngameName | tr '[:lower:]' '[:upper:]')\",>> $Sorce/assetsTMP.h
echo -n $(file -b $1.png | cut -d " " -f 4,6 --output-delimiter ", " | rev | cut -c 2- | rev), >> $Sorce/assetsTMP.h
echo -n $Basename"Bitmap," >> $Sorce/assetsTMP.h
if [ $NumColors -lt 0 ] ; then
  echo -n "NULL," >> $Sorce/assetsTMP.h
else
  echo -n $Basename"Pal," >> $Sorce/assetsTMP.h
fi
echo -n $NumColors"," >> $Sorce/assetsTMP.h
echo -n $TextureFormat, >> $Sorce/assetsTMP.h
echo -n "NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T" >> $Sorce/assetsTMP.h
echo -n "};" >> $Sorce/assetsTMP.h

sed -i "$(wc -l < $Sorce/assets.h)i\\$(cat $Sorce/assetsTMP.h)\\" $Sorce/assets.h
rm $Sorce/assetsTMP.h


sed -i "$(($(grep -n "MaterialMetadata textures" $Sorce/assetsArray.h | cut -f1 -d:)+1))i\\  Material_$FriendlyIngameName,\\" $Sorce/assetsArray.h
sed -i "$(($(grep -n "NE_Material\*\* neMaterials" $Sorce/assetsArray.h | cut -f1 -d:)+1))i\\  &NEMaterial_$FriendlyIngameName,\\" $Sorce/assetsArray.h
sed -i "$(($(grep -n "NE_Palette\*\* nePallettes" $Sorce/assetsArray.h | cut -f1 -d:)+1))i\\  &NEPallette_$FriendlyIngameName,\\" $Sorce/assetsArray.h
sed -i "$(($(grep -n "bool alreadyLoadedTextures" $Sorce/assetsArray.h | cut -f1 -d:)+1))i\\  0,\\" $Sorce/assetsArray.h