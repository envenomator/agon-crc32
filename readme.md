## MOS CRC32 utility
Creates a CRC32 checksum of a given file
### Installation
Install [crc32.bin](https://github.com/envenomator/agon-crc32/releases/latest/download/crc32.bin) in the /bin folder on your SD card. Create this folder first if it's doesn't exist.
### Usage
```
crc32 <filename>
```

### Build from source
The source can be assembled using [agon-ez80asm](https://github.com/AgonPlatform/agon-ez80asm). Obtain a version of the assembler and assemble with
```
ez80asm crc32.s