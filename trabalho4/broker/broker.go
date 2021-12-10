package broker

import (
	"fmt"
	"time"
)

type Broker struct {
	ConsumersBuffers []chan int
	ProducersBuffer  chan int
	ProducersDone    chan bool
	BrokerDone       chan bool
}

func NewBroker(num_producers, num_consumers int, producersBuffer chan int, consumersBuffers []chan int, producersDone chan bool, brokerDone chan bool) *Broker {

	return &Broker{
		ConsumersBuffers: consumersBuffers,
		ProducersBuffer:  producersBuffer,
		ProducersDone:    producersDone,
		BrokerDone:       brokerDone,
	}
}

func (b *Broker) StartListening() {

	// Start listening for new messages in producer channels
LOOP:
	for {
		select {
		case newItem := <-b.ProducersBuffer:
			{
				fmt.Printf("[%d][BROKER] received new item %d. Broadcasting....\n", time.Now().UnixMilli(), newItem)
				b.sendItemToConsumers(newItem)
				continue LOOP
			}
		default:
		}

		select {
		case <-b.ProducersDone:
			{
				fmt.Printf("[%d][BROKER] Received done signal...\n", time.Now().UnixMilli())
				b.finishBroker()
				return
			}
		default:
		}
	}

}

func (b *Broker) sendItemToConsumers(item int) {

	// Publish item to each consumer's buffer
	for _, consumerBuffer := range b.ConsumersBuffers {
		consumerBuffer <- item
	}

}

func (b *Broker) finishBroker() {
	// Close producers buffer
	close(b.ProducersBuffer)

	// Send Done signal to each consumer
	for i := 0; i < len(b.ConsumersBuffers); i++ {
		b.BrokerDone <- true
	}
}
