#include "navigation.h"
#include <string.h>
#include <cmath>
#include <iostream>

const char* Navigation::TAG_NAVI = "Navi";

Navigation::Navigation()
: routeLength(0),
  currentTargetIndex(0),
  headingRad(0.0f)
{
    coordinates[0] = 0.0f;
    coordinates[1] = 0.0f;

    for (int i = 0; i < MAX_ROUTE_LEN; ++i) {
        routeIDs[i].clear();
    }

}

/*FÖRSLAG JSON Derulo:

#include <stdio.h>
#include "cJSON.h"      // Inkludera biblioteket
#include "esp_log.h"

static const char *TAG = "JSON_EXAMPLE";

void app_main(void)
{
    // 1. Din JSON-data (t.ex. från MQTT eller HTTP)
    const char *json_string = "{\"sensor\":\"temp\", \"värde\": 24.5}";

    // 2. Parsa strängen till ett cJSON-objekt (Struct)
    cJSON *root = cJSON_Parse(json_string);
    
    // Alltid bra att kolla så det inte blev fel (t.ex. trasig JSON)
    if (root == NULL) {
        ESP_LOGE(TAG, "Kunde inte parsa JSON");
        return;
    }

    // 3. Hämta ut specifika fält
    cJSON *sensor = cJSON_GetObjectItem(root, "sensor");
    cJSON *value  = cJSON_GetObjectItem(root, "värde");

    // 4. Läs och använd värdena
    // cJSON_GetStringValue är säkert, returnerar strängen eller NULL
    if (cJSON_IsString(sensor)) {
        ESP_LOGI(TAG, "Sensortyp: %s", cJSON_GetStringValue(sensor));
    }

    if (cJSON_IsNumber(value)) {
        // valuedouble används för float/double, valueint för heltal
        ESP_LOGI(TAG, "Temperatur: %.2f", value->valuedouble);
    }

    // 5. VIKTIGT: Städa upp minnet!
    // I C måste du manuellt frigöra minnet som cJSON allokerade.
    // Glömmer du detta får du minnesläckor (memory leaks).
    cJSON_Delete(root); 
}
// Antag att 'root' är hela ditt parsade JSON-objekt
cJSON *noder_lista = cJSON_GetObjectItem(root, "noder");

// Kontrollera att det faktiskt är en array
if (cJSON_IsArray(noder_lista)) {
    
    // Hämta element på index 1 (vilket är den andra noden, "vardagsrum")
    cJSON *vald_nod = cJSON_GetArrayItem(noder_lista, 1);

    if (vald_nod != NULL) {
        cJSON *plats = cJSON_GetObjectItem(vald_nod, "plats");
        ESP_LOGI("JSON", "Vald plats: %s", plats->valuestring);
    }
}


*/


void Navigation::calibrateFromQR(const std::string& qrId){ //strukturera om så att korrigering sker även om oväntat QR avläses
    //this is happening in task. be careful!!!!
    if (qrId.empty()) return;

    ESP_LOGI(TAG_NAVI, "Read qr code: %s", qrId);

    if (qrId == lastId){
        //korrigera?


        return;
    }
    
    if (qrId == routeIDs[currentTargetIndex]) {

       
        currentTargetIndex++; //rätt QR-kod hittad, vidare till nästa

        // TODO: Själva korrigeringen
        //coordinates[0] = map.nodeId.x~isch
        //coordinates[1] = map.nodeId.y~isch

        //logga till server vilket rum? logga massa
        //rapportera om konstverk hittades??
    } else {
        // oväntat QR avläst, kanske loggas? 
        ESP_LOGI(TAG_NAVI, "Unknown qr code!");
    } 

}

void Navigation::getCoordinates(float coords[2]) const{
        coords[0] = coordinates[0];
        coords[1] = coordinates[1];
}

float Navigation::getHeadingDeg() const {
    return headingRad * 180.0f / static_cast<float>(M_PI);
}

void Navigation::updateRoute(const std::string route[], int length) {
    routeLength = 0;
    currentTargetIndex = 0;

    if (length <= 0) return;

    int maxLen = (length < MAX_ROUTE_LEN) ? length : MAX_ROUTE_LEN;

    for (int i = 0; i < maxLen; ++i) {
        routeIDs[i] = route[i]; 
        routeLength++;
    }
}


