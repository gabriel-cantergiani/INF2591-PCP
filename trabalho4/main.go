package main

import (
	"fmt"
	"os"
	"strconv"
	"sync"
	"time"

	brk "github.com/gabriel-cantergiani/INF2591-PCP/broker"
)

func main() {

	if len(os.Args) < 5 {
		fmt.Println("Usage: ./main <buffer_size> <number_of_producers> <number_of_consumers> <number_of_items>")
		os.Exit(1)
	}

	// Initialize variables
	N, err := strconv.Atoi(os.Args[1])
	if err != nil {
		fmt.Println(err)
		os.Exit(2)
	}
	P, err := strconv.Atoi(os.Args[2])
	if err != nil {
		fmt.Println(err)
		os.Exit(2)
	}
	C, err := strconv.Atoi(os.Args[3])
	if err != nil {
		fmt.Println(err)
		os.Exit(2)
	}
	I, err := strconv.Atoi(os.Args[4])
	if err != nil {
		fmt.Println(err)
		os.Exit(2)
	}

	// Create items slice
	items := make([]int, I)
	for i := 0; i < I; i++ {
		items[i] = i
	}

	fmt.Printf("Parameters: N=%d, P=%d, C=%d, I=%d\n", N, P, C, I)

	// Create channels
	producersBuffer := make(chan int, N)
	producersDone := make(chan bool)
	brokerDone := make(chan bool, N)
	var consumersBuffers []chan int
	for i := 0; i < C; i++ {
		newChannel := make(chan int, N)
		consumersBuffers = append(consumersBuffers, newChannel)
	}

	// Create WaitGroup
	producersWg := sync.WaitGroup{}
	consumersWg := sync.WaitGroup{}

	// Create broker
	broker := brk.NewBroker(P, C, producersBuffer, consumersBuffers, producersDone, brokerDone)

	// Start broker
	fmt.Println("[MAIN] Starting broker...\n")
	go broker.StartListening()

	// Start consumers
	fmt.Println("[MAIN] Creating consumers...\n")
	for i := 0; i < C; i++ {
		consumersWg.Add(1)
		go consume(i+1, consumersBuffers[i], brokerDone, &consumersWg)
	}

	// Start producers
	fmt.Println("[MAIN] Creating producers...\n")
	for i := 0; i < P; i++ {
		producersWg.Add(1)
		go produce(i+1, items, producersBuffer, &producersWg)
	}

	fmt.Println("[MAIN] waiting for producers to finish...\n")
	producersWg.Wait()
	// time.Sleep(time.Second * 2)
	producersDone <- true
	consumersWg.Wait()

}

func consume(consumerID int, buffer chan int, done chan bool, wg *sync.WaitGroup) {

	fmt.Printf("[CONSUMER %d] Consumer starting...\n", consumerID)

LOOP:
	for {
		select {
		case item := <-buffer:
			{
				fmt.Printf("[CONSUMER %d] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Received item %d\n", consumerID, item)
				continue LOOP
			}
		default:
		}

		select {
		case <-done:
			{
				fmt.Printf("[CONSUMER %d] Received broker done. Closing channel\n", consumerID)
				close(buffer)
				break LOOP
			}
		default:
		}
	}

	fmt.Printf("[CONSUMER %d] Consumer finishing...\n", consumerID)
	wg.Done()
}

func produce(producerID int, items []int, buffer chan int, wg *sync.WaitGroup) {

	fmt.Printf("[PRODUCER %d]Producer starting...\n", producerID)

	time.Sleep(time.Duration(producerID))
	for item := range items {
		time.Sleep(time.Second * 2)
		value := item + (10 * (producerID - 1))
		fmt.Printf("[PRODUCER %d] Sending item %d\n", producerID, value)
		buffer <- value
	}

	fmt.Printf("[PRODUCER %d] Finished producing....\n", producerID)
	wg.Done()
}
