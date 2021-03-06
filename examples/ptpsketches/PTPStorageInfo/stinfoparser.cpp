#include "stinfoparser.h"

const char* PTPStorageInfoParser::stNames[] PROGMEM =
{
	msgUndefined,
	msgFixedROM,
	msgRemovableROM,	
	msgFixedRAM,		
	msgRemovableRAM	
};

const char* PTPStorageInfoParser::fstNames[] PROGMEM =
{
	msgUndefined,
	msgGenericFlat,			
	msgGenericHierarchical,	
	msgDCF					
};

const char* PTPStorageInfoParser::acNames[] PROGMEM =
{
	msgUndefined,
	msgReadWrite,						
	msgReadOnly,						
	msgReadOnly_with_Object_Deletion
};


void PTPStorageInfoParser::Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset)
{
	uint16_t	cntdn	= (uint16_t)len;
	uint8_t		*p		= (uint8_t*)pbuf;

	switch (nStage)
	{
	case 0:
		p		+= 12;
		cntdn	-= 12;
		nStage ++;
	case 1:
		PrintStorageType(&p, &cntdn);
		nStage ++;
	case 2:
		PrintFileSystemType(&p, &cntdn);
		nStage ++;
	case 3:
		PrintAccessCapability(&p, &cntdn);
		nStage ++;
	case 4:
		PrintMaxCapacity(&p, &cntdn);
		nStage ++;
	case 5:
		PrintFreeSpaceInBytes(&p, &cntdn);
		nStage ++;
	case 6:
		PrintFreeSpaceInImages(&p, &cntdn);
		nStage ++;
	case 7:
		Notify(PSTR("Storage Description:\t"));
		nStage ++;
	case 8:
		if (!PrintString(&p, &cntdn))
			return;
		strByteCountDown = 0;
		nStage ++;
	case 9:
		Notify(PSTR("\nVolume Label:\t"));
		nStage ++;
	case 10:
		if (!PrintString(&p, &cntdn))
			return;
		Notify(PSTR("\n"));
		strByteCountDown = 0;
		nStage ++;
	}
}

void PTPStorageInfoParser::PrintStorageType(uint8_t **pp, uint16_t *pcntdn)
{
	Notify(PSTR("\nStorage Type:\t\t"));
	uint16_t	type = *((uint16_t*)*pp);

	if (type >= 0 && type <= PTP_ST_RemovableRAM)
		Notify((char*)pgm_read_word(&stNames[type]));
	else
		Notify(PSTR("Vendor Defined"));

	Notify(PSTR(" (0x"));
	PrintHex<uint16_t>(type);
	Notify(PSTR(")\n"));

	(*pp)		+= 2;
	(*pcntdn)	-= 2;
}

void PTPStorageInfoParser::PrintFileSystemType(uint8_t **pp, uint16_t *pcntdn)
{
	Notify(PSTR("File System Type:\t"));
	uint16_t	type = *((uint16_t*)*pp);

	if (type >= 0 && type <= PTP_FST_DCF)
		Notify((char*)pgm_read_word(&fstNames[type]));
	else
		Notify(PSTR("Vendor Defined"));

	Notify(PSTR(" (0x"));
	PrintHex<uint16_t>(type);
	Notify(PSTR(")\n"));

	(*pp)		+= 2;
	(*pcntdn)	-= 2;
}

void PTPStorageInfoParser::PrintAccessCapability(uint8_t **pp, uint16_t *pcntdn)
{
	Notify(PSTR("Access Capability:\t"));
	uint16_t	type = *((uint16_t*)*pp);

	if (type >= 0 && type <= PTP_AC_ReadOnly_with_Object_Deletion)
		Notify((char*)pgm_read_word(&acNames[type]));
	else
		Notify(PSTR("Vendor Defined"));

	Notify(PSTR(" (0x"));
	PrintHex<uint16_t>(type);
	Notify(PSTR(")\n"));

	(*pp)		+= 2;
	(*pcntdn)	-= 2;
}

void PTPStorageInfoParser::PrintMaxCapacity(uint8_t **pp, uint16_t *pcntdn)
{
	Notify(PSTR("Max Capacity:\t\t0x"));
	uint32_t	loword = *((uint32_t*)*pp);

	(*pp)		+= 4;
	(*pcntdn)	-= 4;

	uint32_t	hiword = *((uint32_t*)*pp);

	(*pp)		+= 4;
	(*pcntdn)	-= 4;

	PrintHex<uint32_t>(hiword);
	PrintHex<uint32_t>(loword);
	Notify(PSTR("\n"));
}

void PTPStorageInfoParser::PrintFreeSpaceInBytes(uint8_t **pp, uint16_t *pcntdn)
{
	Notify(PSTR("Free Space:\t\t0x"));
	uint32_t	loword = *((uint32_t*)*pp);

	(*pp)		+= 4;
	(*pcntdn)	-= 4;

	uint32_t	hiword = *((uint32_t*)*pp);

	(*pp)		+= 4;
	(*pcntdn)	-= 4;

	PrintHex<uint32_t>(hiword);
	PrintHex<uint32_t>(loword);
	Notify(PSTR(" bytes\n"));
}

void PTPStorageInfoParser::PrintFreeSpaceInImages(uint8_t **pp, uint16_t *pcntdn)
{
	Notify(PSTR("Free Space:\t\t0x"));
	uint32_t	loword = *((uint32_t*)*pp);

	(*pp)		+= 4;
	(*pcntdn)	-= 4;

	PrintHex<uint32_t>(loword);
	Notify(PSTR(" images\n"));
}

bool PTPStorageInfoParser::PrintString(uint8_t **pp, uint16_t *pcntdn)
{
	if (!(*pcntdn))
		return true;

	if ( !strByteCountDown )
	{
		if ( !(**pp) )
		{
			(*pp) ++;
			(*pcntdn) --;
			return true;
		}
		strByteCountDown = ((**pp) << 1);
		(*pp) ++;
		(*pcntdn) --;
	}

	for (; strByteCountDown && (*pcntdn); strByteCountDown--, (*pcntdn)--, (*pp)++)
	{
		if ( !(strByteCountDown & 1) && ((**pp) > 0))
			Serial.print((unsigned char)(**pp));
	}
	return (strByteCountDown == 0);
}
