#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

struct valeurCapteurs {
    int analogique;
    int numerique;
    double tempsEnMillisecondes;
};

//Initialisation des queues
QueueHandle_t xQueue1; 
QueueHandle_t xQueue2;
QueueHandle_t xQueue3;
QueueHandle_t xQueue4;

//Initialisation des variables
int PIN_2;
int PIN_3;
int valeur_analogique;
int valeur_numerique;
valeurCapteurs valeur;

//Initialisation des tâches
void tache1(void*);
void tache2(void*);
void tache3(void*);
void tache4(void*);
void tache5(void*);

//Initialisation d'un mutex
SemaphoreHandle_t xMutexVar = NULL;

//Initialisation d'une fonction d'affichage
void affichage(String);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIN_2, INPUT);
  pinMode(PIN_3, INPUT);

  xQueue1 = xQueueCreate(10, sizeof(int));
  xQueue2 = xQueueCreate(10, sizeof(int));
  xQueue3 = xQueueCreate(10, sizeof(valeurCapteurs));
  xQueue4 = xQueueCreate(10, sizeof(valeurCapteurs));
  
}

void loop() {
  // put your main code here, to run repeatedly:

}



void tache1(void* p)
{
  while(1)
  {
    valeur_analogique = analogRead(A0); //Récupère la valeur sur l'entrée A0
    xQueueSendToBack(xQueue1, &valeur_analogique, portMAX_DELAY); //Pour renvoyer l'information
  }
  vTaskDelay(1000 / portTICK_PERIOD_MS); //on attend une seconde
}

void tache2(void* p)
{
  while(1)
  {
    if((digitalRead(PIN_2) == HIGH))
    {
      valeur_numerique++;
    }
    else
    {
      valeur_numerique = valeur_numerique + 0;
    }
    if((digitalRead(PIN_3) == HIGH))
    {
      valeur_numerique++;
    }
    else
    {
      valeur_numerique = valeur_numerique + 0;
    }
    xQueueSendToBack(xQueue2, &valeur_numerique, portMAX_DELAY);
    vTaskDelay(1000 / portTICK_PERIOD_MS); //on attend une seconde
    valeur_numerique = 0;
  }
}

void tache3(void* p)
{
  while(1)
  {
    if ((xQueueReceive( xQueue1, &valeur.analogique, portMAX_DELAY ) == pdTRUE) && (xQueueReceive( xQueue2, &valeur.numerique, portMAX_DELAY ) == pdTRUE))
    {
      valeur.tempsEnMillisecondes = millis();
      xQueueSendToBack(xQueue3, &valeur, portMAX_DELAY);
    }
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); //on attend une seconde
  }
}

void tache4(void* p)
{
  while(1)
  {
    if((xQueueReceive( xQueue3, &valeur, portMAX_DELAY ) == pdTRUE))
    {
      xQueueSendToBack(xQueue4, &valeur, portMAX_DELAY);
      affichage("valeur analogique : ");
      affichage((String)valeur.analogique);
      affichage((String)'\n');
      affichage("valeur numerique : ");
      affichage((String)valeur.numerique);
      affichage((String)'\n');
      affichage("valeur du temps en milisecondes : ");
      affichage((String)valeur.tempsEnMillisecondes);
      affichage((String)'\n');
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); //on attend une seconde
  }
}

void tache5(void* p)
{
  while(1)
  {
    if((xQueueReceive( xQueue4, &valeur.analogique, portMAX_DELAY ) == pdTRUE))
    {
      affichage("temps en minutes : ");
      affichage((String)(valeur.tempsEnMillisecondes/60000)); //on converti le temps en minutes
      affichage("min");
      affichage((String)'\n');
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); //on attend une seconde
  }
}

void affichage(String chaine)
{
  if( xSemaphoreTake(xMutexVar, portMAX_DELAY) == pdTRUE)
  {
    Serial.print(chaine);
    xSemaphoreGive(xMutexVar); // release mutex
  }
 
}
