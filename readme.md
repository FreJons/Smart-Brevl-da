# README för Smart Brevlåda

## Kort Sammanfattning
Detta projekt uppstod efter upprepade tillfällen då viktig post nästan gick förlorad. För att lösa problemet utvecklade jag en systemlösning som använder en magnetkontakt monterad på brevinkastet för att skicka realtidsnotiser till Discord när post levereras. Systemet integrerar en ESP32 mikrokontroller med AWS, där data flödar genom IoT-Core, Timestream, och en Lambda-funktion för att skapa notiser. Denna lösning minimerar risken för bortglömd eller förlorad post och har resulterat i ett lyckat projekt.
