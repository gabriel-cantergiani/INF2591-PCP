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
	fmt.Printf("[%d][MAIN] Starting broker...\n", time.Now().UnixMilli())
	go broker.StartListening()

	// Start consumers
	fmt.Printf("[%d][MAIN] Creating consumers...\n", time.Now().UnixMilli())
	for i := 0; i < C; i++ {
		consumersWg.Add(1)
		go consume(i+1, consumersBuffers[i], brokerDone, &consumersWg)
	}

	// Start producers
	fmt.Printf("[%d][MAIN] Creating producers...\n", time.Now().UnixMilli())
	for i := 0; i < P; i++ {
		producersWg.Add(1)
		go produce(i+1, items, producersBuffer, &producersWg)
	}

	fmt.Printf("[%d][MAIN] waiting for producers to finish...\n", time.Now().UnixMilli())
	producersWg.Wait()
	producersDone <- true
	consumersWg.Wait()

}

func consume(consumerID int, buffer chan int, done chan bool, wg *sync.WaitGroup) {

	fmt.Printf("[%d][CONSUMER %d] Consumer starting...\n", time.Now().UnixMilli(), consumerID)

LOOP:
	for {
		select {
		case item := <-buffer:
			{
				fmt.Printf("[%d][CONSUMER %d] Received item %d\n", time.Now().UnixMilli(), consumerID, item)
				continue LOOP
			}
		default:
		}

		select {
		case <-done:
			{
				fmt.Printf("[%d][CONSUMER %d] Received broker done. Closing channel\n", time.Now().UnixMilli(), consumerID)
				close(buffer)
				break LOOP
			}
		default:
		}
	}

	fmt.Printf("[%d][CONSUMER %d] Consumer finishing...\n", time.Now().UnixMilli(), consumerID)
	wg.Done()
}

func produce(producerID int, items []int, buffer chan int, wg *sync.WaitGroup) {

	fmt.Printf("[%d][PRODUCER %d]Producer starting...\n", time.Now().UnixMilli(), producerID)

	time.Sleep(time.Second)
	for item := range items {
		buffer <- item
		fmt.Printf("[%d][PRODUCER %d] Sent item %d\n", time.Now().UnixMilli(), producerID, item)
		time.Sleep(time.Millisecond * 500)
	}

	fmt.Printf("[%d][PRODUCER %d] Finished producing....\n", time.Now().UnixMilli(), producerID)
	wg.Done()
}
