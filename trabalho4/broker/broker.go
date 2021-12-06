package broker

import "fmt"

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
		// fmt.Printf("[BROKER] starting loop...\n")
		select {
		case newItem := <-b.ProducersBuffer:
			{
				fmt.Printf("[BROKER] received new item %d. Broadcasting....\n", newItem)
				b.sendItemToConsumers(newItem)
				continue LOOP
			}
		default:
		}

		select {
		case <-b.ProducersDone:
			{
				fmt.Printf("[BROKER] Received done signal...\n")
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
