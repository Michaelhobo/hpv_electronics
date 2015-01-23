#include "mbed.h"
#include "nRF24L01P.h"

Serial pc(USBTX, USBRX); // tx, rx
nRF24L01P my_nrf24l01p(p5, p6, p7, p8, p9, p10);    // mosi, miso, sck, csn, ce, irq

int main() {
#define TRANSFER_SIZE 10

	char rxData[TRANSFER_SIZE];
	my_nrf24l01p.powerUp();
	my_nrf24l01p.setRfFrequency (2501);
	my_nrf24l01p.setTransferSize(TRANSFER_SIZE);
	my_nrf24l01p.setCrcWidth(8);
	my_nrf24l01p.enableAutoAcknowledge(NRF24L01P_PIPE_P0);
	my_nrf24l01p.setRxAddress(0x00F0F0F0F0);
	//my_nrf24l01p.setTxAddress(0xF0F0F0F0D2);
	// Display the setup of the nRF24L01+ chip
	pc.printf( "nRF24L01+ Frequency    : %d MHz\r\n",  my_nrf24l01p.getRfFrequency() );
	pc.printf( "nRF24L01+ Data Rate    : %d kbps\r\n", my_nrf24l01p.getAirDataRate() );
	pc.printf( "nRF24L01+ TX Address   : 0x%010llX\r\n", my_nrf24l01p.getTxAddress() );
	pc.printf( "nRF24L01+ RX Address   : 0x%010llX\r\n", my_nrf24l01p.getRxAddress() );
	pc.printf( "nRF24L01+ CrC Width    : %d CrC\r\n", my_nrf24l01p.getCrcWidth() );
	pc.printf( "nRF24L01+ TransferSize : %d Packet Size\r\n", my_nrf24l01p.getTransferSize () );
	my_nrf24l01p.setReceiveMode();
	my_nrf24l01p.enable();
	pc.printf( "TEST: Setup complete, Starting While loop\r\n");

	while (1) {
		if(my_nrf24l01p.readable(NRF24L01P_PIPE_P0)){
			pc.printf("read %d!\r\n", sizeof(rxData));
			do{
				my_nrf24l01p.read( NRF24L01P_PIPE_P0, rxData, sizeof( rxData ) );
				//int i;
				//for (i = 0; i > 0; i ++) {
				//	pc.printf("Vals : %d\n\r", rxData[i]);  
				//}    
				pc.printf(rxData);
			}while(my_nrf24l01p.readable(NRF24L01P_PIPE_P0));
		}                                           
	}
}
