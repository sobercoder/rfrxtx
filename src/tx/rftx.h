/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * rftx.h
 *
 * Created: 04-Sep-2012 9:42:49 PM
 * Author: Sandipan Das
 */

#ifndef RFTX_H_
#define RFTX_H_

#define BAUDRATE			2400
#define UBRRVAL				((F_CPU/(BAUDRATE*16UL))-1)

#define PACKET_HEAD	0xAA	// header
#define PACKET_SIGN	0x2E	// signature
#define CMD_PWR		0x01	// toggle power on/off
#define CMD_INC		0x02	// increment speed
#define CMD_DEC		0x03	// decrement speed
#define CRC_CMD_PWR	0xDF
#define CRC_CMD_INC	0x3D
#define CRC_CMD_DEC	0x63

void rftx_init(void);
void tx_putcmd(uint8_t data);

#endif /* RFTX_H_ */
