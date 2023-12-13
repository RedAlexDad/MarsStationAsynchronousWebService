package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"math/rand"
	"net/http"
	"time"
)

const (
	MarsStationToken = "my_secret_token"
	resultURL        = "http://127.0.0.1:8000/api/async_result/"
)

type MarsStationResult struct {
	IDDraft       int    `json:"id_draft"`
	StatusMission int    `json:"status_mission"`
	Token         string `json:"token"`
}

type RequestBody struct {
	IDDraft int `json:"id_draft"`
}

// Функция для преобразования числа в соответствующее слово
func getStatusWord(status int) string {
	switch status {
	case 0:
		return "Ошибка"
	case 1:
		return "В работе"
	case 2:
		return "Успех"
	case 3:
		return "Потеря"
	default:
		return "Неизвестный статус"
	}
}

func main() {
	http.HandleFunc("/api/async_calc/", handleProcess)
	fmt.Println("Server running at port :8100")
	http.ListenAndServe(":8100", nil)
}

func handleProcess(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "The method is not allowed", http.StatusMethodNotAllowed)
		return
	}

	var requestBody RequestBody
	decoder := json.NewDecoder(r.Body)
	if err := decoder.Decode(&requestBody); err != nil {
		http.Error(w, "Error decoding JSON", http.StatusBadRequest)
		fmt.Println("Error decoding JSON:", err)
		return
	}

	id_draft := requestBody.IDDraft
	fmt.Println(id_draft)

	// Генерируем случайное число в диапазоне [0.0, 1.0)
	randomValue := rand.Float64()

	var status_mission int
	// 70% шанс для Успеха
	if randomValue < 0.7 {
		status_mission = 2
		// 30% шанс для Потери
	} else {
		status_mission = 3
	}

	// Успешный ответ в формате JSON
	successMessage := map[string]interface{}{
		"message":        "Successful",
		"status_mission": getStatusWord(status_mission),
		"data": MarsStationResult{
			IDDraft:       id_draft,
			StatusMission: status_mission,
			Token:         MarsStationToken,
		},
	}

	jsonResponse, err := json.Marshal(successMessage)
	if err != nil {
		http.Error(w, "Error encoding JSON", http.StatusInternalServerError)
		fmt.Println("Error encoding JSON:", err)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusOK)
	w.Write(jsonResponse)

	go func() {
		// Задержка 5 секунда
		delay := 5
		time.Sleep(time.Duration(delay) * time.Second)

		// Отправка результата на другой сервер
		result := MarsStationResult{
			IDDraft:       id_draft,
			StatusMission: status_mission,
			Token:         MarsStationToken,
		}

		fmt.Println("json", result)
		jsonValue, err := json.Marshal(result)
		if err != nil {
			fmt.Println("Error during JSON marshalization:", err)
			return
		}

		req, err := http.NewRequest(http.MethodPut, resultURL, bytes.NewBuffer(jsonValue))
		if err != nil {
			fmt.Println("Error when creating an update request:", err)
			return
		}
		req.Header.Set("Content-Type", "application/json")

		client := &http.Client{}
		resp, err := client.Do(req)
		if err != nil {
			fmt.Println("Error when sending an update request:", err)
			return
		}
		defer resp.Body.Close()

		fmt.Println("Response from the update server:", resp.Status)
	}()
}
