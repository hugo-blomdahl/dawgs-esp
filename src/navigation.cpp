#include "navigation.h" 
#include <cmath>  
#include "cJSON.h" 
#include "esp_log.h"
#include "map_json.h"

const char* TAG_NAVI = "Navi";

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

bool Navigation::loadEmbeddedMap() {
    if (map_json_len == 0) {
        ESP_LOGE(TAG_NAVI, "Embedded map är tom!");
        return false;
    }

    // Skapa modifierbar buffer och null-terminera (cJSON kräver C-sträng)
    std::vector<char> buf;
    buf.reserve(map_json_len + 1);
    buf.insert(buf.end(), map_json, map_json + map_json_len);
    buf.push_back('\0');

    return loadMapFromJson(std::string(buf.data()));
}


bool Navigation::loadMapFromJson(const std::string& jsonString) { 
    mapNodes.clear();

    cJSON *root = cJSON_Parse(jsonString.c_str());
    if (root == NULL) {
        ESP_LOGE(TAG_NAVI, "Kunde inte parsa JSON-kartdata.");
        return false;
    }
    
    cJSON *nodes_array = cJSON_GetObjectItemCaseSensitive(root, "nodes");
    if (!cJSON_IsArray(nodes_array)) {
        ESP_LOGE(TAG_NAVI, "JSON saknar 'nodes' array.");
        cJSON_Delete(root);
        return false;
    }

    int nodeCount = cJSON_GetArraySize(nodes_array);
    
    for (int i = 0; i < nodeCount; i++) {
        cJSON *node_obj = cJSON_GetArrayItem(nodes_array, i);
        if (node_obj) {
            SimpleNode node;
            
            // Läs koordinater
            cJSON *x = cJSON_GetObjectItemCaseSensitive(node_obj, "x");
            cJSON *y = cJSON_GetObjectItemCaseSensitive(node_obj, "y");
            
            // Läs QRID
            cJSON *qr_item = cJSON_GetObjectItemCaseSensitive(node_obj, "qrId"); 

            bool validCoords = cJSON_IsNumber(x) && cJSON_IsNumber(y);
            bool validQR = cJSON_IsString(qr_item) && (qr_item->valuestring != nullptr);

            if (validCoords) {
                node.x = (float)x->valuedouble;
                node.y = (float)y->valuedouble;
            } else {
                ESP_LOGW(TAG_NAVI, "Nod %d saknar giltiga koordinater, ignoreras.", i);
                continue;
            }

            if (validQR) {
                node.qrId = qr_item->valuestring; // lagrar QR-ID som string
            } else {
                node.qrId = ""; // tom string om QR saknas
            }

            mapNodes.push_back(node);
        }
    }

    cJSON_Delete(root);
    ESP_LOGI(TAG_NAVI, "Karta laddad och parsas. %zu noder lagrade i minnet.", mapNodes.size());
    return true;
}


void Navigation::calibrateFromQR(int qrIndex){ 
    if (qrIndex < 0 || qrIndex >= mapNodes.size()) {
        ESP_LOGE(TAG_NAVI, "Ogiltigt QR index %d. Utanför kartans område.", qrIndex);
        return;
    }
    
    const SimpleNode& targetNode = mapNodes[qrIndex];
    
 
    coordinates[0] = targetNode.x;
    coordinates[1] = targetNode.y;
    ESP_LOGI(TAG_NAVI, "Calibration COMPLETE. Index %d -> (%.2f, %.2f)", 
             qrIndex, coordinates[0], coordinates[1]);

    std::string qrIdStr = targetNode.qrId; 

    // Om samma QR läses igen, gör inget
    if (qrIdStr.empty() || qrIdStr == lastId) {
        return;
    }

    updateRouteProgress(qrIdStr);

    lastId = qrIdStr;
}


void Navigation::updateRouteProgress(const std::string& qrId) {
    if (currentTargetIndex < routeLength && qrId == routeIDs[currentTargetIndex]) {
        currentTargetIndex++;
        ESP_LOGI(TAG_NAVI, "Route progress: Target found. Moving to next index %d.", currentTargetIndex);
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