#include <esp_now.h>;
#include <WiFi.h>;

// Definir endereço da placa comunicadora
uint8_t broadcastAddress[] = {0xD8, 0xBC, 0x38, 0xE2, 0x48, 0x9C};

// Buffer de memória
String buffer[9];

// Difinição do padrão da mensagem emitida
typedef struct {
  String emissor_Prod;
  String emissor_Req;
} dado_emissor;

typedef struct {
  String recebedor_Prod;
  String recebedor_Req;
} dado_recebedor;

dado_emissor dadoE;
dado_recebedor dadoR;

esp_now_peer_info_t peerInfo;

int i;

SemaphoreHandle_t mutex = xSemaphoreCreateMutex();

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
  modeStation();
  InitESPNow();

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
  // mensagem de armazenamento concluido
}

void RecebeDados(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&dadoR, incomingData, sizeof(dadoR));
  

  if (dadoE.emissor_Req == "POST")
  {
    xSemaphoreTake(mutex, portMAX_DELAY);
    insereLetra(dadoR); // Se o buffer estiver com espaço livre
    xSemaphoreGive(mutex);
  }
  else if (dadoE.emissor_Req == "GET")
  {
    xSemaphoreTake(mutex, portMAX_DELAY);
    retiraLetra(dadoR, dadoE); // Se o consumidor solicitar uma letra
    xSemaphoreGive(mutex);
  }

  // Exibe estado buffer
  exibeBuffer();
}

void insereLetra(dado_recebedor dado)
{
  if (i < 9) // Se houver espaço
  {
    for (int l = 0; l < 9; l++) // Percorre o buffer
    {
      if (buffer[l] == "") // Se posição vazia
      {
        buffer[l] = dado.recebedor_Prod;
        i++;
        Serial.println("Letra armazenada na posição ");
        Serial.print(l);
        break;
      }
    }
  }
  else
  {
    Serial.println("Sem espaço para armazenar a letra!");
  }
};

void retiraLetra(dado_recebedor dadoRecebido, dado_emissor dadoEnviado)
{
  if (i != 0)
  {
    for (int k = 0; k < 9; k++)
    {
      if (buffer[k] == dadoRecebido.recebedor_Prod)
      {
        buffer[k] = "";
        i--;
        Serial.println("Produto retirado da posição ");
        Serial.print(k);
        respostaConsumidor(dadoEnviado, dadoRecebido);
        // Função de envio do produto;
        enviaResLetra(dadoEnviado);
        break;
      }
    }
  }
  else
  {
    Serial.println("Estoque Vazio!");
  }
}

void respostaConsumidor(dado_emissor dadoEnviar, dado_recebedor produtoEstoque ) {
  dadoEnviar.emissor_Prod = produtoEstoque.recebedor_Prod;
  Serial.println("Produto alocado na mensagem de envio");
  dadoEnviar.emissor_Req = "RES";
}

void enviaResLetra(dado_emissor dado) {
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &dado, sizeof(dado));
}

void exibeBuffer()
{
  for (int j = 0; j < 9; j++)
  {
    Serial.print("Buffer: ");
    Serial.println(buffer[j]);
  }
}

void InitESPNow(){
    if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow iniciou com successo");
  }
  else {
    Serial.println("ESPNow falhou na inicializaÃ§Ã£o");
    ESP.restart();
  }
}

void modeStation(){
  WiFi.mode(WIFI_STA);
  Serial.print("EndereÃ§o MAC placa ESP:     ");
  Serial.print(WiFi.macAddress());
}
