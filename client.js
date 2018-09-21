// import net from 'net'
const net = require('net')

const socket = new net.Socket()
socket.on('connect', () => {
  console.log('connected!')
  socket.write(
    JSON.stringify({
      ctrl: {
        xRot: 0,
        yRot: 0,
        zRot: 0,
        xLin: 0,
        yLin: 0,
        zLin: 0
      }
    })
  )
})
socket.on('data', data => {
  debugger
  console.log(data.toString())
})
socket.connect(
  '23',
  '192.168.1.191'
)
