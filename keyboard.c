/*
 * Tastaturtreiber
 *	Version 0.1a vom 29/04/2016
 *  by Jan - Frederic Kurzweil
 * Funktionsumfang
 * - keyboard_init
 * 		leerlauf, falls der Puffer leer sein sollte
 * - FetchAndAnalyzeScancode
 *		Analyse des Scancode, es wird geprüft ob die Shift Tate 
 *		gedrückt wurde und gibt dann den richtigen ASCII code zuürck um das Zeichen darstellen zukönnen
 * - kgetch
 * 		öffentliche Funktion zum einlesen von zeichen von der Tastatur es wird ein ASCII Code zurück gegeben
 * - keyboard_handler
 * 		testhandler um zu prüfen ob Tastatur eingaben erkannt und korrekt verarbeitet werden
 * - keyboard_install //TODO: herraus finden was der genau wo rein installiert wird
 */
 
#include "keyboard_DEde.h"
#include "os.h"

int ShiftKeyDown;															// Variable für ShiftKeyDown

void keyboard_init()														// Warte falls der Puffer leer ist
{
	while (inportb(0x64) & 1)
		inportb(0x60);
}

unsigned int FetchAndAnalyzeScancode()
{
	uint scancode;															// tastatur scancode
	while(TRUE)																// Schleife nach dem eine (w/o shift key) Taste gedrückt wurde
	{
		scancode = inportb(0x60);											// 0x60 lese scancode von der Tastatur
		if( scancode & 0x80 )												// Wurde die Taste los gelassen ? prüfe Bit 7 (100000000b = 0x80) des scancodes 
		{
			scancode &= 0x7F;												// Taste wurde losgelassen, verarbeite nur die 7 low bits (01111111b = 0x7F) 
			if(scancode == KRLEFT_SHIFT || scancode == KRRIGHT_SHIFT)		// prüfe ob shift gedrückt
			{
				ShiftKeyDown = 0;											// Nein Shift wurde nicht gedrückt --> NonShift
			}
		}
		else																// Taste wurde gedrückt
		{
			if ( scancode == KRLEFT_SHIFT || KRRIGHT_SHIFT )				// erfasse scancode der shift taste, falls diese gedrückt wurde
			{
				ShiftKeyDown = 1;											// Falls die taste gedrückt wurde, benutze die asciiShift Zeichen
				continue;													// schleife damit nicht der scancode oder shift taste zurück gegeben wird
			}
		}
		break;																// verlasse die schleife
	}
	return scancode;
}

void uchar const kgetch()													// Scancode --> ASCII
{
	uint scan;																// Scancode von der Tastatur
	uchar retchar;															// retchar --> rückgabe wert des scnacode als ASCII Code
	scan = FetchAndAnalyzeScancode();										// hol den scancode und analysiere ob Shift gedrückt wurde
	if(ShiftKeyDown)
		retchar = asciiShift[scan];											// gedrückter ShiftCode
	else
		retchar = asciiNonShift[scan];										// nichtgedrückter ShiftCode
	return retchar;															// ASCII Version
}

void keyboard_handler(struct regs* r)
{
	kprintf("keyboard handöer successfull init", 8, 0x2);
	uchar bufferKey[10];
	bufferKey[0] = kgetch();
	kprintf(bufferKey, 10, 0xA);
}

void keyboard_install()														// Installiere den "Keyboard_Handler" in irq1
{
	irq_install_handler(1, keyboard_handler);
	keyboard_init();
}