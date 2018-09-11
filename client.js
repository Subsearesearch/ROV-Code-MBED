// import net from 'net'
const net = require('net')

const socket = new net.Socket()
socket.on('connect', () => {
  console.log('connected!')
  socket.write(
    JSON.stringify({
      ctrl: {
        quat: { w: 0.3681245526846781, x: 0.9297764858882513, y: 0, z: 0 },
        lin: { x: 0, y: 0.45, z: 0 },
        pq: 1,
        pw: 1
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
