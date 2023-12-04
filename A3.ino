#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = { 0xD8, 0xBC, 0x38, 0xE2, 0x48, 0x9C };
String buffer[9];

typedef struct struct_message {
  String S;
  String R;
} struct_message;

int i;

struct_message myData;

esp_now_peer_info_t peerInfo;

// Reponde quando dados enviados
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\Status do ultimo pacote enviado:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Successo na entrega" : "Falha na entrega");
}


void setup() {
  // Iniciar Monitor  Serial
  Serial.begin(115200);
  Serial.println();
  Serial.print("EndereÃ§o MAC placa ESP:     ");
  Serial.print(WiFi.macAddress());
  // Configura dispositio como EstaÃ§Ã£o Wi-Fi
  WiFi.mode(WIFI_STA);

  // Inicia ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao inicializar ESP-NOW");
    return;
  }

  // recebe status do pacote enviado
  esp_now_register_send_cb(OnDataSent);

  // Registrar peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Adicionar peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Falha ao adicionar peer");
    return;
  }
}

void loop() {
}

void RecebeDados(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Byte Recebido:      ");
  Serial.println(len);
  Serial.print("Letra S:    ");
  Serial.println(myData.S);
  Serial.print("Letra R:     ");
  Serial.println(myData.R);

  // Se o buffer estiver com espaço livre
  espacoLivre();

  //Se o consumidor solicitar uma letra
  if (i != 0) {
    for (int k = 0; k < 9; k++) {
      if (buffer[k] == myData.S) {
        Serial.print(buffer[k]);
        buffer[k] = "";
        i--;
        break;
      }
    }
  }

  //Exibe estado buffer
  exibeBuffer();
}

void espacoLivre() {
  if (i < 9) {
    for (int l = 0; l < 9; l++) {
      if (buffer[l] == "") {
        buffer[i] = myData.R;
        i++;
        break;
      }
    }
  }
}

void exibeBuffer() {
  for (int j = 0; j < 9; j++) {
    Serial.print("Buffer: ");
    Serial.println(buffer[j]);
  }
}