// import net from 'net'
const net = require('net')

const socket = new net.Socket()
socket.on('connect', () => {
  console.log('connected!')
})
socket.on('data', data => {
  debugger
  console.log(data)
})
socket.connect(
  '23',
  '192.168.1.191'
)
