#include "navigation.h" 
#include <cmath>  
#include "cJSON.h" 
#include "esp_log.h"
#include "map_json.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char* TAG_NAVI = "Navi";

// Konstruktor
Navigation::Navigation(Movement* movement)
: movementController(movement),
  activeRouteType(0),
  currentPathIndex(0),
  currentRouteLength(0)
{
}

bool Navigation::loadEmbeddedMap() {
    if (map_json_len == 0) {
        ESP_LOGE(TAG_NAVI, "Embedded map är tom!");
        return false;
    }
    std::vector<char> buf;
    buf.reserve(map_json_len + 1);
    buf.insert(buf.end(), map_json, map_json + map_json_len);
    buf.push_back('\0');

    return loadMapFromJson(std::string(buf.data()));
}

void Navigation::parsePathArray(cJSON* nodeObj, const char* arrayName, std::vector<NavAction>& targetVector) {
    cJSON *pathArray = cJSON_GetObjectItemCaseSensitive(nodeObj, arrayName);
    
    if (cJSON_IsArray(pathArray)) {
        int steps = cJSON_GetArraySize(pathArray);
        for (int j = 0; j < steps; j++) {
            cJSON *step = cJSON_GetArrayItem(pathArray, j);
            cJSON *action = cJSON_GetObjectItemCaseSensitive(step, "do");
            cJSON *val = cJSON_GetObjectItemCaseSensitive(step, "val");

            if (cJSON_IsString(action) && cJSON_IsNumber(val)) {
                NavAction navAct;
                navAct.value = (float)val->valuedouble;
                
                std::string actStr = action->valuestring;
                if (actStr == "F") navAct.type = ACTION_FORWARD;
                else if (actStr == "B") navAct.type = ACTION_BACKWARD;
                else if (actStr == "R") navAct.type = ACTION_ROTATE;
                else continue; 

                targetVector.push_back(navAct);
            }
        }
    }
}

bool Navigation::loadMapFromJson(const std::string& jsonString) { 
    mapNodes.clear();

    cJSON *root = cJSON_Parse(jsonString.c_str());
    if (root == NULL) {
        ESP_LOGE(TAG_NAVI, "Kunde inte parsa JSON.");
        return false;
    }
    
    cJSON *nodes_array = cJSON_GetObjectItemCaseSensitive(root, "nodes");
    if (!cJSON_IsArray(nodes_array)) {
        cJSON_Delete(root);
        return false;
    }

    int nodeCount = cJSON_GetArraySize(nodes_array);
    
    for (int i = 0; i < nodeCount; i++) {
        cJSON *node_obj = cJSON_GetArrayItem(nodes_array, i);
        if (node_obj) {
            SimpleNode node;
            
            cJSON *id_item = cJSON_GetObjectItemCaseSensitive(node_obj, "id"); 
            node.id = cJSON_IsNumber(id_item) ? id_item->valueint : -1;
            
            cJSON *art_item = cJSON_GetObjectItemCaseSensitive(node_obj, "art");
            node.art = (art_item && art_item->valuestring) ? art_item->valuestring : "";
            
            parsePathArray(node_obj, "pathRoute5", node.route5Actions);
            parsePathArray(node_obj, "pathRoute6", node.route6Actions);
            
            mapNodes.push_back(node);
        }
    }

    cJSON_Delete(root);
    ESP_LOGI(TAG_NAVI, "Karta laddad. %zu noder lagrade.", mapNodes.size());
    return true;
}


std::string Navigation::getNodeArt(int qrId) {
    if (qrId >= 0 && qrId < mapNodes.size()) {
        return mapNodes[qrId].art;
    }
    return ""; 
}


void Navigation::setRoute(int routeType, const int* pathList, int length) {
    activeRouteType = routeType;
    currentPathIndex = 0; 
    
    if (length > MAX_ROUTE_CAPACITY) {
        ESP_LOGW(TAG_NAVI, "Varning: Rutt för lång (%d). Kapar till %d.", length, MAX_ROUTE_CAPACITY);
        length = MAX_ROUTE_CAPACITY;
    }

    currentRouteLength = length;

    for (int i = 0; i < length; i++) {
        currentRoutePath[i] = pathList[i];
    }
    
    ESP_LOGI(TAG_NAVI, "Rutt satt (Array)! Typ: %d, Längd: %d. Start: %d", 
             activeRouteType, currentRouteLength, 
             (length > 0) ? currentRoutePath[0] : -1);
}

void Navigation::calibrateFromQR(int qrIndex){ 
    if (currentRouteLength == 0) {
        return;
    }

    if (currentPathIndex >= currentRouteLength) {
        ESP_LOGI(TAG_NAVI, "Rutt redan klar.");
        return;
    }

    int expectedQrId = currentRoutePath[currentPathIndex];

    if (qrIndex == expectedQrId) {
        
    
        if (qrIndex >= 0 && qrIndex < mapNodes.size()) {
            const SimpleNode& node = mapNodes[qrIndex];
            
            if (node.id != qrIndex) {
                 ESP_LOGW(TAG_NAVI, "Varning: Index %d har ID %d i kartan. Omatch!", qrIndex, node.id);
            }

            ESP_LOGI(TAG_NAVI, "MATCH! QR %d hittad. Utför rutt %d.", qrIndex, activeRouteType);

            if (activeRouteType == 5) {
                executePathSequence(node.route5Actions);
            } else if (activeRouteType == 6) {
                executePathSequence(node.route6Actions);
            }
        } else {
            ESP_LOGE(TAG_NAVI, "QR %d ligger utanför kartans gränser (IndexOutOfBounds).", qrIndex);
        }

        currentPathIndex++;
        
        if (currentPathIndex >= currentRouteLength) {
            ESP_LOGI(TAG_NAVI, "--- RUTT SLUTFÖRD ---");
            currentRouteLength = 0; 
        } else {
            ESP_LOGI(TAG_NAVI, "Nästa mål: QR %d", currentRoutePath[currentPathIndex]);
        }
    }
    
    lastId = std::to_string(qrIndex);
}

void Navigation::executePathSequence(const std::vector<NavAction>& actions) {
    if (!movementController) return;

    for (const auto& action : actions) {
        
        movementController->setSpeed(50); 
        
        const float SIM_STEP_M = 0.1f;    
        const int STEP_DELAY_MS = 100;    

        if (action.type == ACTION_FORWARD) {
            ESP_LOGI(TAG_NAVI, "EXEC: Framåt %.2f m", action.value);
            float dist = 0;
            while(dist < action.value) {
                movementController->moveForward();
                dist += SIM_STEP_M;
                vTaskDelay(pdMS_TO_TICKS(STEP_DELAY_MS));
            }

        } else if (action.type == ACTION_BACKWARD) {
            ESP_LOGI(TAG_NAVI, "EXEC: Bakåt %.2f m", action.value);
            float dist = 0;
            while(dist < action.value) {
                movementController->moveBackward();
                dist += SIM_STEP_M;
                vTaskDelay(pdMS_TO_TICKS(STEP_DELAY_MS));
            }

        } else if (action.type == ACTION_ROTATE) {
            ESP_LOGI(TAG_NAVI, "EXEC: Rotera %.2f grader", action.value);
            movementController->rotate(action.value);
            vTaskDelay(pdMS_TO_TICKS(1000)); 
        }

        movementController->standby();
        vTaskDelay(pdMS_TO_TICKS(200)); 
    }
}