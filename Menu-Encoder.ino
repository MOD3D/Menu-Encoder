#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <I2C_RTC.h>
static DS3231 RTC;
// Definir la dirección del I2C del LCD
#define LCD_ADDR 0x27
// Configuración de la pantalla LCD y del encoder
LiquidCrystal_I2C lcd(LCD_ADDR, 20, 4);
Encoder myEnc(23, 24);
// Configuración del botón del encoder
#define BUTTON_PIN 3
long oldPosition  = -999;
int menuIndex = 0;

// Definición de los elementos del menú y submenús
const char* mainMenu[] = {"Simulacion", "Parametrizacion", "Salir"};
const char* simMenu[] = {"Modulo Calor", "Modulo Frio", "Atras"};
const char* simModMenu[] = {"Iniciar Simulacion", "Datos", "Atras"};
const char* paramMenu[] = {"Tiempo", "SP Temp", "Vel. Aire", "Humedad", "Temperatura", "Presion", "Vel. Viento", "Verificar Datos", "Salir"};
const char* yesNoMenu[] = {"SI", "NO"};

const int mainMenuLength = sizeof(mainMenu) / sizeof(mainMenu[0]);
const int simMenuLength = sizeof(simMenu) / sizeof(simMenu[0]);
const int simModMenuLength = sizeof(simModMenu) / sizeof(simModMenu[0]);
const int paramMenuLength = sizeof(paramMenu) / sizeof(paramMenu[0]);
const int yesNoMenuLength = sizeof(yesNoMenu) / sizeof(yesNoMenu[0]);

const char** currentMenu = mainMenu;
int currentMenuLength = mainMenuLength;
bool inSubMenu = false;
bool inMainMenu = false;

void setup() {
  // Configurar el pin del botón
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // Inicializar la pantalla LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  // Mostrar la pantalla de inicio
  menu_E_Inactivo();
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!inMainMenu) {
      inMainMenu = true;
      displayMenu();
    } else {
      handleMenuSelection();
    }
    delay(500);
  }
  if (inMainMenu) {
    // Leer la posición del encoder
    long newPosition = myEnc.read() / 4;
    // Si la posición ha cambiado, actualizar el índice del menú y mostrar el menú
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      menuIndex = newPosition % currentMenuLength;
      if (menuIndex < 0) {
        menuIndex += currentMenuLength;
      }
      displayMenu();
    }
  }
}

void menu_E_Inactivo() {
  lcd.setCursor(3, 0);
  lcd.print("Estado Inactivo");
  lcd.setCursor(4, 1);
  lcd.print("Fecha y Hora");
  lcd.setCursor(1, 2);
  lcd.print(RTC.getDay());
  lcd.print("/");
  lcd.print(RTC.getMonth());
  lcd.print("/");
  lcd.print(RTC.getYear());
  lcd.print("--");
  lcd.print(RTC.getHours());
  lcd.print(":");
  lcd.print(RTC.getMinutes());
  lcd.print(":");
  lcd.print(RTC.getSeconds());
  lcd.setCursor(4, 3);
  lcd.print("Oprima Menu");
}

void displayMenu() {
  // Limpiar la pantalla
  lcd.clear();
  // Mostrar los elementos del menú
  for (int i = 0; i < currentMenuLength; i++) {
    lcd.setCursor(1, i);
    lcd.print(currentMenu[i]);
  }
  // Mostrar el cursor en el ítem seleccionado
  for (int i = 0; i < currentMenuLength; i++) {
    if (i == menuIndex) {
      lcd.setCursor(0, i);
      lcd.print(">");
    } else {
      lcd.setCursor(0, i);
      lcd.print(" ");
    }
  }
}

void handleMenuSelection() {
  if (currentMenu == mainMenu) {
    switch (menuIndex) {
      case 0:
        currentMenu = simMenu;
        currentMenuLength = simMenuLength;
        menuIndex = 0;
        break;
      case 1:
        currentMenu = paramMenu;
        currentMenuLength = paramMenuLength;
        menuIndex = 0;
        break;
      case 2:
        menu_E_Inactivo();
        inMainMenu = false;
        return;
    }
  } else if (currentMenu == simMenu) {
    switch (menuIndex) {
      case 0:
      case 1:
        currentMenu = simModMenu;
        currentMenuLength = simModMenuLength;
        menuIndex = 0;
        break;
      case 2:
        currentMenu = mainMenu;
        currentMenuLength = mainMenuLength;
        menuIndex = 0;
        break;
    }
  } else if (currentMenu == simModMenu) {
    if (menuIndex == simModMenuLength - 1) {
      currentMenu = simMenu;
      currentMenuLength = simMenuLength;
      menuIndex = 0;
    } else {
      Serial.print("Simulacion Módulo - ");
      Serial.println(simModMenu[menuIndex]);
    }
  } else if (currentMenu == paramMenu) {
    if (menuIndex == paramMenuLength - 1) {
      currentMenu = mainMenu;
      currentMenuLength = mainMenuLength;
      menuIndex = 0;
    } else {
      Serial.print("Parametrizacion - ");
      Serial.println(paramMenu[menuIndex]);
    }
  } else if (currentMenu == yesNoMenu) {
    if (menuIndex == yesNoMenuLength - 1) {
      currentMenu = mainMenu;
      currentMenuLength = mainMenuLength;
      menuIndex = 0;
    } else {
      Serial.print("Yes/No - ");
      Serial.println(yesNoMenu[menuIndex]);
    }
  }
  displayMenu();
}
