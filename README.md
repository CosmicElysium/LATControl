# LATControl
Control software for a Laser Alignment Telescope, similar to that currently installed on the Large Binocular Telescope. 

# Required Packages
libzeroc-ice-dev
libcfitsio-dev

# Pre-Installation
Download PvAPI from Allied Vision Technologies
```
cd LATControl
wget https://cdn.alliedvision.com/fileadmin/content/software/software/PvAPI/PvAPI_1.28_Linux.tgz
tar xf PvAPI_1.28_Linux.tgz
mv AVT\ GigE\ SDK/* LATServer/lib/
```

# Installation
```
cd LATControl
make
```
