#pragma once

#define LGFX_USE_V1

#include <LovyanGFX.hpp>

//*/
// Esempio di impostazione quando si utilizza LovyanGFX con impostazioni uniche su ESP32

/*
Copia questo file, assegnagli un nuovo nome e modifica le impostazioni per adattarle al tuo ambiente.
Può essere utilizzato includendo il file creato dal programma utente.

Puoi utilizzare il file duplicato inserendolo nella cartella lgfx_user della libreria, ma
In tal caso, tieni presente che potrebbe essere eliminato durante l'aggiornamento della libreria.

Se vuoi operare in sicurezza, crea un backup o inseriscilo nella cartella del progetto dell'utente.
//*/
//----------------------------------------------------------------------
// https://github.com/lovyan03/LovyanGFX/blob/master/examples/HowToUse/2_user_setting/2_user_setting.ino


#if defined(DISPLAY_PANEL_ST7789)
static constexpr uint8_t DASHBOARD_DISPLAY_ROTATION = 1;
static constexpr uint8_t DASHBOARD_TOUCH_ROTATION_OFFSET = 4;
#else
static constexpr uint8_t DASHBOARD_DISPLAY_ROTATION = 3;
static constexpr uint8_t DASHBOARD_TOUCH_ROTATION_OFFSET = 6;
#endif

class LGFX : public lgfx::LGFX_Device{
#if defined(DISPLAY_PANEL_ST7789)
  lgfx::Panel_ST7789 _panel_instance;
#else
  lgfx::Panel_ILI9341 _panel_instance;
#endif
  lgfx::Bus_SPI       _bus_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::Touch_XPT2046 _touch_instance;
//----------------------------------------------------------------------  
public:LGFX(void){
  {                            // Configurare le impostazioni di controllo del bus.
  auto cfg = _bus_instance.config();// Ottieni la struttura per le impostazioni del bus.
                               // Configurazione del bus SPI

  cfg.spi_host   = SPI2_HOST;  // 使用するSPIを選択 (VSPI_HOST or HSPI_HOST)
  cfg.spi_mode = 0; // Imposta la modalità di comunicazione SPI (0 ~ 3)
  cfg.freq_write = 40000000; // Orologio SPI durante la trasmissione (massimo 80 MHz, arrotondando 80 MHz a un valore intero)
  cfg.freq_read = 16000000; // Orologio SPI durante la ricezione
  cfg.spi_3wire = false; // Imposta su true se la ricezione avviene tramite pin MOSI
  cfg.use_lock = true; // Imposta true per utilizzare il blocco delle transazioni
  cfg.dma_channel= 1; // Impostazione canale DMA (1or2,0=disabilita) (0=DMA non utilizzato)
  cfg.pin_sclk = 14; // Imposta il numero pin SPI SCLK SCK
  cfg.pin_mosi = 13; // Imposta il numero pin SPI MOSI SDI
  cfg.pin_miso = 12; // Imposta il numero pin SPI MISO (-1 = disabilita) SDO
  cfg.pin_dc = 2; // Imposta il numero pin D/C SPI (-1 = disabilita) RS

  //Se utilizzi lo stesso bus SPI della scheda SD, assicurati di impostare MISO senza ometterlo.
  _bus_instance.config(cfg);   // Riflette il valore di impostazione sul bus.
  _panel_instance.setBus(&_bus_instance);// Posiziona l'autobus sul pannello.
  }
  {                            // 表示パネル制御の設定を行います。
  auto cfg = _panel_instance.config();// Ottieni la struttura per le impostazioni del pannello di visualizzazione.
  cfg.pin_cs = 15; // Numero pin a cui è collegato CS (-1 = disabilitato)
  cfg.pin_rst = 12; // Numero pin a cui è collegato RST (-1 = disabilitato)
  cfg.pin_busy = -1; // Numero pin a cui è collegato BUSY (-1 = disabilitato)
  cfg.memory_width = 240; // Larghezza massima supportata dall'IC del driver
  cfg.memory_height = 320; // Altezza massima supportata dal driver IC
  cfg.panel_width = 240; // Larghezza effettiva visualizzabile
  cfg.panel_height = 320; // Altezza effettiva di visualizzazione
  cfg.offset_x = 0; // Quantità di offset della direzione X del pannello
  cfg.offset_y = 0; // Quantità di offset della direzione Y del pannello
  cfg.offset_rotation = 0; // Offset del valore della direzione di rotazione 0~7 (4~7 sono capovolti)
  cfg.dummy_read_pixel= 8; // Numero di bit per la lettura fittizia prima della lettura dei pixel
  cfg.dummy_read_bits = 1; // Numero di bit per la lettura fittizia prima della lettura dei dati all'esterno del pixel
  cfg.readable = true; // Imposta su true se è possibile la lettura dei dati
  cfg.invert = false; // Imposta su true se la luminosità del pannello è invertita
  cfg.rgb_order = false; // Imposta su true se il rosso e il blu sul pannello vengono scambiati, ok
  cfg.dlen_16bit = false; // Pannello che invia i dati in unità di 16 bit Impostato su true
  cfg.bus_shared = false; // Condividi il bus con la scheda SD Impostato su true
  _panel_instance.config(cfg);
  }
  { // Configurare le impostazioni di controllo della retroilluminazione. (Cancellare se non necessario)
  auto cfg = _light_instance.config();// Ottieni la struttura per la configurazione della retroilluminazione.
  cfg.pin_bl = 21;             // Numero del pin BL a cui è collegata la retroilluminazione
  cfg.invert = false;          // true per invertire la luminosità della retroilluminazione
  cfg.freq   = 44100;          // Frequenza PWM della retroilluminazione
  cfg.pwm_channel = 7;         // Numero del canale PWM da utilizzare
  _light_instance.config(cfg);
  _panel_instance.setLight(&_light_instance);//Posiziona la retroilluminazione sul pannello.
  }
  { // Configurare le impostazioni di controllo del touch screen. (Cancellare se non necessario)
  auto cfg = _touch_instance.config();
  cfg.x_min = 300; // Valore X minimo ottenuto dal touch screen (valore grezzo)
  cfg.x_max = 3900; // Valore X massimo ottenuto dal touch screen (valore grezzo)
  cfg.y_min = 200; // Valore Y minimo ottenuto dal touch screen (valore grezzo)
  cfg.y_max = 3700; // Valore Y massimo ottenuto dal touch screen (valore grezzo)
  cfg.pin_int = -1; // Numero pin a cui è collegato INT, TP IRQ 36
  cfg.bus_shared = false; // Imposta true se si utilizza lo stesso bus dello schermo
  cfg.offset_rotation = DASHBOARD_TOUCH_ROTATION_OFFSET; // Regolazione quando la direzione del display e del tocco non corrispondono Impostare con un valore compreso tra 0 e 7
  // Per la connessione SPI
  cfg.spi_host = VSPI_HOST;// 使Seleziona la SPI da utilizzare (HSPI_HOST o VSPI_HOST)
  cfg.freq = 1000000;      // Seleziona la SPI da utilizzare (HSPI_HOST o VSPI_HOST)
  cfg.pin_sclk = 25;       // SCLK Numero pin a cui è collegato TP CLK
  cfg.pin_mosi = 32;       // MOSI Numero pin a cui è collegato, TP DIN
  cfg.pin_miso = 39;       // MISO Numero pin a cui è collegato, TP DOUT
  cfg.pin_cs   = 33;       // CS  Numero pin a cui è collegato, TP CS
  _touch_instance.config(cfg);
  _panel_instance.setTouch(&_touch_instance);  // Posizionare il touch screen sul pannello.
  }
  setPanel(&_panel_instance);// Impostare il pannello da utilizzare.
  }
};

//=====================================================================

