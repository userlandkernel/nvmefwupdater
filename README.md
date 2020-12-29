# nvmefwupdater

This is totally not reversed from /usr/libexec/nvmefwupdater...   
All credit for this code belongs to Apple Inc, California.

This took me minor work but it's part of a bigger project of research on the iPhone's NAND chip, in particular the one found in iPhone 6S.  
If you want to support this project or wanna contribute with some food to go along the research u can: https://paypal.me/devsupport.

**WORK IS IN PROGRESS**

![Current state of RE](img/Screen%20Shot%202020-12-13%20at%2017.02.29.png)

## A little bit of explaining
- The main branch currently contains the code for iPhone 6S and it should support all NAND chips for that device.  


### Getting the firmware [Read more at Ramtin Amin Web Blog](https://jaytaylor.com/notes/node/1479404824000.html)
```
wget http://appldnld.apple.com/iOS9.3/031-28353-20160321-9904D9BA-EA47-11E5-9069-7170BD379832/com_apple_MobileAsset_SoftwareUpdate/352b7321dda0925f84c7beb6128a3dbdb71476fe.zip;
unzip 352b7321dda0925f84c7beb6128a3dbdb71476fe.zip AssetData/payload/replace/usr/standalone/update/ramdisk/H8SURamDisk.dmg;
dd if=AssetData/payload/replace/usr/standalone/update/ramdisk/H8SURamDisk.dmg of=update.dmg bs=27 skip=1;
file update.dmg;
mkdir d;
mount -o loop,ro update.dmg d;

```

### Fun fax
- On iOS < 13.5 you may be able to update the nand without a jailbreak by using this exploit by Siguza: https://siguza.github.io/psychicpaper/ for the entitlement in this repository xD
