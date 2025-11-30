;
; Title:	AGON MOS - API for user projects
; Author:	Dean Belfield
; Created:	03/08/2022
; Last Updated:	11/11/2025
; Adjusted for fasmg assembler (Paul Cawte, 16/06/2023)
; Adjusted for MOS 3.0 API (Jeroen Venema, 18/04/2025)
;
; Modinfo:
; 05/08/2022:	Added mos_feof
; 09/08/2022:	Added system variables: cursorX, cursorY
; 18/08/2022:	Added system variables: scrchar, scrpixel, audioChannel, audioSuccess, vdp_pflags
; 05/09/2022:	Added mos_ren, vdp_pflag_mode
; 24/09/2022:	Added mos_getError, mos_mkdir
; 13/10/2022:	Added mos_oscli
; 23/02/2023:	Added more sysvars, fixed typo in sysvar_audioSuccess, offsets for sysvar_scrCols, sysvar_scrRows
; 04/03/2023:	Added sysvar_scrpixelIndex
; 08/03/2023:	Renamed sysvar_keycode to sysvar_keyascii, added sysvar_vkeycode
; 15/03/2023:	Added mos_copy, mos_getrtc, mos_setrtc, rtc, vdp_pflag_rtc
; 21/03/2023:	Added mos_setintvector, sysvars for keyboard status, vdu codes for vdp
; 22/03/2023:	The VDP commands are now indexed from 0x80
; 29/03/2023:	Added mos_uopen, mos_uclose, mos_ugetc, mos_uputc
; 13/04/2023:	Added FatFS file structures (FFOBJID, FIL, DIR, FILINFO)
; 15/04/2023:	Added mos_getfil, mos_fread, mos_fwrite and mos_flseek
; 18/11/2023:	Added _mos_setkbvector, _mos_getkbmap, _mos_i2c_open, _mos_i2c_close, _mos_i2c_write, _mos_i2c_read
; 18/04/2025:   Added remaining ffs_* API values in MOS 2.20 / 3.0
; 11/11/2025:   Added 0x27+ sysvar offsets

; VDP control (VDU 23, 0, n)
;
vdp_gp:             EQU 080h
vdp_keycode:        EQU 081h
vdp_cursor:         EQU 082h
vdp_scrchar:        EQU 083h
vdp_scrpixel:       EQU 084h
vdp_audio:          EQU 085h
vdp_mode:           EQU 086h
vdp_rtc:            EQU 087h
vdp_keystate:       EQU 088h
vdp_logicalcoords:  EQU 0C0h
vdp_terminalmode:   EQU 0FFh

; MOS high level functions
;
mos_getkey:                EQU 000h
mos_load:                  EQU 001h
mos_save:                  EQU 002h
mos_cd:                    EQU 003h
mos_dir:                   EQU 004h
mos_del:                   EQU 005h
mos_ren:                   EQU 006h
mos_mkdir:                 EQU 007h
mos_sysvars:               EQU 008h
mos_editline:              EQU 009h
mos_fopen:                 EQU 00Ah
mos_fclose:                EQU 00Bh
mos_fgetc:                 EQU 00Ch
mos_fputc:                 EQU 00Dh
mos_feof:                  EQU 00Eh
mos_getError:              EQU 00Fh
mos_oscli:                 EQU 010h
mos_copy:                  EQU 011h
mos_getrtc:                EQU 012h
mos_setrtc:                EQU 013h
mos_setintvector:          EQU 014h
mos_uopen:                 EQU 015h
mos_uclose:                EQU 016h
mos_ugetc:                 EQU 017h
mos_uputc:                 EQU 018h
mos_getfil:                EQU 019h
mos_fread:                 EQU 01Ah
mos_fwrite:                EQU 01Bh
mos_flseek:                EQU 01Ch
mos_setkbvector:           EQU 01Dh
mos_getkbmap:              EQU 01Eh
mos_i2c_open:              EQU 01Fh
mos_i2c_close:             EQU 020h
mos_i2c_write:             EQU 021h
mos_i2c_read:              EQU 022h
mos_unpackrtc:             EQU 023h
mos_flseek_p:              EQU 024h
mos_pmatch:                EQU 028h
mos_getargument:           EQU 029h
mos_extractstring:         EQU 02Ah
mos_extractnumber:         EQU 02Bh
mos_escapestring:          EQU 02Ch
mos_setvarval:             EQU 030h
mos_readvarval:            EQU 031h
mos_gsinit:                EQU 032h
mos_gsread:                EQU 033h
mos_gstrans:               EQU 034h
mos_substituteargs:        EQU 035h
mos_evaluateexpression:    EQU 036h
mos_resolvepath:           EQU 038h
mos_getdirforpath:         EQU 039h
mos_getleafname:           EQU 03Ah
mos_isdirectory:           EQU 03Bh
mos_getabsolutepath:       EQU 03Ch
mos_clearvdpflags:         EQU 040h
mos_waitforvdpflags:       EQU 041h
mos_getfunction:           EQU 050h
sd_getunlockcode:          EQU 070h
sd_init:                   EQU 071h
sd_readblocks:             EQU 072h
sd_writeblocks:            EQU 073h

; FatFS file access functions
;
ffs_fopen:        EQU 080h
ffs_fclose:       EQU 081h
ffs_fread:        EQU 082h
ffs_fwrite:       EQU 083h
ffs_flseek:       EQU 084h
ffs_ftruncate:    EQU 085h
ffs_fsync:        EQU 086h
ffs_fforward:     EQU 087h
ffs_fexpand:      EQU 088h
ffs_fgets:        EQU 089h
ffs_fputc:        EQU 08Ah
ffs_fputs:        EQU 08Bh
ffs_fprintf:      EQU 08Ch
ffs_ftell:        EQU 08Dh
ffs_feof:         EQU 08Eh
ffs_fsize:        EQU 08Fh
ffs_ferror:       EQU 090h

; FatFS directory access functions
;
ffs_dopen:        EQU 091h
ffs_dclose:       EQU 092h
ffs_dread:        EQU 093h
ffs_dfindfirst:   EQU 094h
ffs_dfindnext:    EQU 095h

; FatFS file and directory management functions
;
ffs_stat:         EQU 096h
ffs_unlink:       EQU 097h
ffs_rename:       EQU 098h
ffs_chmod:        EQU 099h
ffs_utime:        EQU 09Ah
ffs_mkdir:        EQU 09Bh
ffs_chdir:        EQU 09Ch
ffs_chdrive:      EQU 09Dh
ffs_getcwd:       EQU 09Eh

; FatFS volume management and system configuration functions
;
ffs_mount:        EQU 09Fh
ffs_mkfs:         EQU 0A0h
ffs_fdisk:        EQU 0A1h
ffs_getfree:      EQU 0A2h
ffs_getlabel:     EQU 0A3h
ffs_setlabel:     EQU 0A4h
ffs_setcp:        EQU 0A5h
ffs_flseek_p:     EQU 0A6h

; File access modes
;
fa_read:          EQU 001h
fa_write:         EQU 002h
fa_open_existing: EQU 000h
fa_create_new:    EQU 004h
fa_create_always: EQU 008h
fa_open_always:   EQU 010h
fa_open_append:   EQU 030h

; System variable indexes for api_sysvars
; Index into _sysvars in globals.asm
;
sysvar_time:           EQU 000h  ; 4: Clock timer in centiseconds (incremented by 2 every VBLANK)
sysvar_vdp_pflags:     EQU 004h  ; 1: Flags to indicate completion of VDP commands
sysvar_keyascii:       EQU 005h  ; 1: ASCII keycode, or 0 .if no key is pressed
sysvar_keymods:        EQU 006h  ; 1: Keycode modifiers
sysvar_cursorX:        EQU 007h  ; 1: Cursor X position
sysvar_cursorY:        EQU 008h  ; 1: Cursor Y position
sysvar_scrchar:        EQU 009h  ; 1: Character read from screen
sysvar_scrpixel:       EQU 00Ah  ; 3: Pixel data read from screen (R,B,G)
sysvar_audioChannel:   EQU 00Dh  ; 1: Audio channel
sysvar_audioSuccess:   EQU 00Eh  ; 1: Audio channel note queued (0 EQU no, 1 EQU yes)
sysvar_scrWidth:       EQU 00Fh  ; 2: Screen width in pixels
sysvar_scrHeight:      EQU 011h  ; 2: Screen height in pixels
sysvar_scrCols:        EQU 013h  ; 1: Screen columns in characters
sysvar_scrRows:        EQU 014h  ; 1: Screen rows in characters
sysvar_scrColours:     EQU 015h  ; 1: Number of colours displayed
sysvar_scrpixelIndex:  EQU 016h  ; 1: Index of pixel data read from screen
sysvar_vkeycode:       EQU 017h  ; 1: Virtual key code from FabGL
sysvar_vkeydown:       EQU 018h  ; 1: Virtual key state from FabGL (0=up, 1=down)
sysvar_vkeycount:      EQU 019h  ; 1: Incremented every time a key packet is received
sysvar_rtc:            EQU 01Ah  ; 8: Real time clock data
sysvar_keydelay:       EQU 022h  ; 2: Keyboard repeat delay
sysvar_keyrate:        EQU 024h  ; 2: Keyboard repeat reat
sysvar_keyled:         EQU 026h  ; 1: Keyboard LED status
sysvar_scrMode:        EQU 027h  ; 1: Screen mode
sysvar_rtcEnable:      EQU 028h  ; 1: RTC enable flag (0: disabled, 1: use ESP32 RTC)
sysvar_mouseX:         EQU 029h  ; 2: Mouse X position
sysvar_mouseY:         EQU 02Bh  ; 2: Mouse Y position
sysvar_mouseButtons:   EQU 02Dh  ; 1: Mouse left+right+middle buttons (bits 0-2, 0=up, 1=down)
sysvar_mouseWheel:     EQU 02Eh  ; 1: Mouse wheel delta
sysvar_mouseXDelta:    EQU 02Fh  ; 2: Mouse X delta
sysvar_mouseYDelta:    EQU 031h  ; 2: Mouse Y delta

; Flags for the VDP protocol
;
vdp_pflag_cursor:   EQU 0b00000001
vdp_pflag_scrchar:  EQU 0b00000010
vdp_pflag_point:    EQU 0b00000100
vdp_pflag_audio:    EQU 0b00001000
vdp_pflag_mode:     EQU 0b00010000
vdp_pflag_rtc:      EQU 0b00100000

;
; FatFS structures
; These mirror the structures contained in src_fatfs/ff.h in the MOS project
;
; Object ID and allocation information (FFOBJID)
;
; Indexes into FFOBJID structure
ffobjid_fs:			EQU	0	; 3: Pointer to the hosting volume of this object
ffobjid_id:			EQU	3	; 2: Hosting volume mount ID
ffobjid_attr:		EQU	5	; 1: Object attribute
ffobjid_stat:		EQU	6	; 1: Object chain status (b1-0: =0:not contiguous, =2:contiguous, =3:fragmented in this session, b2:sub-directory stretched)
ffobjid_sclust:		EQU	7	; 4: Object data start cluster (0:no cluster or root directory)
ffobjid_objsize:	EQU	11	; 4: Object size (valid when sclust != 0)
;
; File object structure (FIL)
;
; Indexes into FIL structure
fil_obj:		EQU 0	; 15: Object identifier
fil_flag:		EQU	15 	;  1: File status flags
fil_err:		EQU	16	;  1: Abort flag (error code)
fil_fptr:		EQU	17	;  4: File read/write pointer (Zeroed on file open)
fil_clust:		EQU	21	;  4: Current cluster of fpter (invalid when fptr is 0)
fil_sect:		EQU	25	;  4: Sector number appearing in buf[] (0:invalid)
fil_dir_sect:	EQU	29	;  4: Sector number containing the directory entry
fil_dir_ptr:	EQU	33	;  3: Pointer to the directory entry in the win[]
;
; Directory object structure (DIR)
; Indexes into DIR structure
dir_obj:		EQU  0	; 15: Object identifier
dir_dptr:		EQU	15	;  4: Current read/write offset
dir_clust:		EQU	19	;  4: Current cluster
dir_sect:		EQU	23	;  4: Current sector (0:Read operation has terminated)
dir_dir:		EQU	27	;  3: Pointer to the directory item in the win[]
dir_fn:			EQU	30	; 12: SFN (in/out) {body[8],ext[3],status[1]}
dir_blk_ofs:	EQU	42	;  4: Offset of current entry block being processed (0xFFFFFFFF:Invalid)
;
; File information structure (FILINFO)
;
; Indexes into FILINFO structure
filinfo_fsize:		EQU 0	;   4: File size
filinfo_fdate:		EQU	4	;   2: Modified date
filinfo_ftime:		EQU	6	;   2: Modified time
filinfo_fattrib:	EQU	8	;   1: File attribute
filinfo_altname:	EQU	9	;  13: Alternative file name
filinfo_fname:		EQU	22	; 256: Primary file name
;
; Macro for calling the API
; Parameters:
; - function: One of the function numbers listed above
;
	MACRO	MOSCALL	function
			LD	A, function
			RST.LIL	08h
	ENDMACRO 	

; MOS program exit codes
;
EXIT_OK:				EQU  0;	"OK",
EXIT_ERROR_SD_ACCESS:	EQU	 1;	"Error accessing SD card",
EXIT_ERROR_ASSERTION:	EQU  2;	"Assertion failed",
EXIT_SD_CARDFAILURE:	EQU  3;	"SD card failure",
EXIT_FILENOTFOUND:		EQU  4;	"Could not find file",
EXIT_PATHNOTFOUND:		EQU  5;	"Could not find path",
EXIT_INVALIDPATHNAME:	EQU  6;	"Invalid path name",
EXIT_ACCESSDENIED_FULL:	EQU  7;	"Access denied or directory full",
EXIT_ACCESSDENIED:		EQU  8;	"Access denied",
EXIT_INVALIDOBJECT:		EQU  9;	"Invalid file/directory object",
EXIT_SD_WRITEPROTECTED:	EQU 10;	"SD card is write protected",
EXIT_INVALIDDRIVENUMBER:EQU 11;	"Logical drive number is invalid",
EXIT_NOVOLUMEWORKAREA:	EQU 12;	"Volume has no work area",
EXIT_NOVALIDFATVOLUME:	EQU 13;	"No valid FAT volume",
EXIT_ERRORMKFS:			EQU 14;	"Error occurred during mkfs",
EXIT_VOLUMETIMEOUT:		EQU 15;	"Volume timeout",
EXIT_VOLUMELOCKED:		EQU 16;	"Volume locked",
EXIT_LFNALLOCATION:		EQU 17;	"LFN working buffer could not be allocated",
EXIT_MAXOPENFILES:		EQU 18;	"Too many open files",
EXIT_INVALIDPARAMETER:	EQU 19;	"Invalid parameter",
EXIT_INVALIDCOMMAND:	EQU 20;	"Invalid command",
EXIT_INVALIDEXECUTABLE:	EQU 21;	"Invalid executable",
