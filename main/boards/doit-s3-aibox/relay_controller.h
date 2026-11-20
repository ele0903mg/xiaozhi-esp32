#ifndef __RELAY_CONTROLLER_H__
#define __RELAY_CONTROLLER_H__

#include "mcp_server.h"
#include <driver/gpio.h>
#include <esp_log.h>
#include <string>

#define TAG "RelayController"

/**
 * 8-channel relay controller exposed to the LLM through MCP tools.
 * 
 * Relay modules are typically ACTIVE LOW:
 *   GPIO LOW  (0) -> relay energizes (switch closes, appliance ON)
 *   GPIO HIGH (1) -> relay de-energizes (switch opens, appliance OFF)
 *
 * All channels are driven HIGH at boot so no appliance turns on unexpectedly
 * after a reboot or during a firmware update.
 */
class RelayController {
private:
    static const int CHANNEL_COUNT = 8;
    gpio_num_t gpios_[CHANNEL_COUNT];
    bool states_[CHANNEL_COUNT];   // true = relay ON (energized)
    bool active_low_;

    void SetLevel(int channel, bool on) {
        if (channel < 1 || channel > CHANNEL_COUNT) {
            return;
        }
        int idx = channel - 1;
        // active_low: ON -> LOW(0), OFF -> HIGH(1)
        // active_high: ON -> HIGH(1), OFF -> LOW(0)
        int level = active_low_ ? (on ? 0 : 1) : (on ? 1 : 0);
        gpio_set_level(gpios_[idx], level);
        states_[idx] = on;
    }

    std::string StatesJson() {
        std::string s = "{";
        for (int i = 0; i < CHANNEL_COUNT; i++) {
            if (i) s += ",";
            s += "\"" + std::to_string(i + 1) + "\":";
            s += states_[i] ? "true" : "false";
        }
        s += "}";
        return s;
    }

public:
    /**
     * @param gpio_pins Array of 8 gpio_num_t, one per relay channel (1..8).
     * @param active_low true for the common low-level-trigger 5V relay boards.
     */
    RelayController(const gpio_num_t* gpio_pins, bool active_low = true)
        : active_low_(active_low) {
        for (int i = 0; i < CHANNEL_COUNT; i++) {
            gpios_[i] = gpio_pins[i];
            states_[i] = false;

            if (gpios_[i] == GPIO_NUM_NC) {
                continue;
            }

            gpio_config_t config = {
                .pin_bit_mask = (1ULL << gpios_[i]),
                .mode = GPIO_MODE_OUTPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_DISABLE,
            };
            ESP_ERROR_CHECK(gpio_config(&config));

            // SAFETY: drive to the OFF level at boot so nothing energizes on start.
            int off_level = active_low_ ? 1 : 0;
            gpio_set_level(gpios_[i], off_level);
        }

        auto& mcp = McpServer::GetInstance();

        // Turn on one channel. Range 1-8 is enforced by the MCP framework.
        mcp.AddTool("self.relay.turn_on",
            "Turn ON one relay channel (power on an appliance). channel is 1-8.",
            PropertyList({ Property("channel", kPropertyTypeInteger, 1, 8) }),
            [this](const PropertyList& properties) -> ReturnValue {
                int ch = properties["channel"].value<int>();
                SetLevel(ch, true);
                ESP_LOGI(TAG, "Relay %d turned ON", ch);
                return true;
            });

        // Turn off one channel.
        mcp.AddTool("self.relay.turn_off",
            "Turn OFF one relay channel (power off an appliance). channel is 1-8.",
            PropertyList({ Property("channel", kPropertyTypeInteger, 1, 8) }),
            [this](const PropertyList& properties) -> ReturnValue {
                int ch = properties["channel"].value<int>();
                SetLevel(ch, false);
                ESP_LOGI(TAG, "Relay %d turned OFF", ch);
                return true;
            });

        // Turn off all channels - emergency / safety command.
        mcp.AddTool("self.relay.turn_off_all",
            "Turn OFF all relay channels at once. Use when asked to turn everything off.",
            PropertyList(),
            [this](const PropertyList& properties) -> ReturnValue {
                for (int i = 1; i <= CHANNEL_COUNT; i++) SetLevel(i, false);
                ESP_LOGI(TAG, "All relays turned OFF");
                return true;
            });

        // Query current state of all channels.
        mcp.AddTool("self.relay.get_state",
            "Get the current on/off state of all 8 relay channels.",
            PropertyList(),
            [this](const PropertyList& properties) -> ReturnValue {
                return StatesJson();
            });
    }
};

#endif // __RELAY_CONTROLLER_H__
