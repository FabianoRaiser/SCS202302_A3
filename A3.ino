#include <esp_now.h>
#include <WiFi.h>

// Definir endereço da placa comunicadora
uint8_t broadcastAddress[] = {0xD8, 0xBC, 0x38, 0xE2, 0x48, 0x9C};

// Buffer de memória
String buffer[9];

// Difinição do padrão da mensagem recebida
typedef struct struct_message
{
  String req;
  String S;
  String R;
} struct_message;
struct_message myData;

esp_now_peer_info_t peerInfo;

int i;

// Reponde quando dados enviados
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\Status do ultimo pacote enviado:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Successo na entrega" : "Falha na entrega");
}

void setup()
{
  // Iniciar Monitor  Serial
  Serial.begin(115200);
  Serial.println();
  Serial.print("EndereÃ§o MAC placa ESP:     ");
  Serial.print(WiFi.macAddress());
  // Configura dispositio como EstaÃ§Ã£o Wi-Fi
  WiFi.mode(WIFI_STA);

  // Inicia ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
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
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Falha ao adicionar peer");
    return;
  }
}

void loop()
{
}

void RecebeDados(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Byte Recebido:      ");
  Serial.println(len);
  Serial.print("Letra S:    ");
  Serial.println(myData.S);
  Serial.print("Letra R:     ");
  Serial.println(myData.R);

  if (myData.req == "POST")
  {
    insereLetra(); // Se o buffer estiver com espaço livre
  }
  else if (myData.req == "GET")
  {
    retiraLetra(); // Se o consumidor solicitar uma letra
  }

  // Exibe estado buffer
  exibeBuffer();
}

void insereLetra()
{
  if (i < 9) // Se houver espaço
  {
    for (int l = 0; l < 9; l++) // Percorre o buffer
    {
      if (buffer[l] == "") // Se posição vazia
      {
        buffer[l] = myData.R;
        i++;
        Serial.println("Produto armazenado na posição ");
        Serial.print(l);
        break;
      }
      
    }
  }
  else {
    Serial.println("Sem espaço para armazenar o produto!");
    }
}

void retiraLetra()
{
  if (i != 0)
  {
    for (int k = 0; k < 9; k++)
    {
      if (buffer[k] == myData.S)
      {
        buffer[k] = "";
        i--;
        Serial.println("Produto retirado da posição ");
        Serial.print(k);
        // Função de envio do produto;
        break;
      }
    }
  }
  else {
    Serial.println("Estoque Vazio!")
  }

}

void exibeBuffer()
{
  for (int j = 0; j < 9; j++)
  {
    Serial.print("Buffer: ");
    Serial.println(buffer[j]);
  }
}