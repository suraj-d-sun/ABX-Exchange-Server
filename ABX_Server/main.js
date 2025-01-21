const net = require('net');

const packetData = {
    packetStream: [
        { symbol: 'MSFT', buysellindicator: 'B', quantity: 50, price: 100, packetSequence: 1 },
        { symbol: 'MSFT', buysellindicator: 'B', quantity: 30, price: 98, packetSequence: 2 },
        { symbol: 'MSFT', buysellindicator: 'S', quantity: 20, price: 101, packetSequence: 3 },
        { symbol: 'MSFT', buysellindicator: 'S', quantity: 10, price: 102, packetSequence: 4 },
        { symbol: 'AAPL', buysellindicator: 'B', quantity: 40, price: 50, packetSequence: 5 },
        { symbol: 'AAPL', buysellindicator: 'S', quantity: 30, price: 55, packetSequence: 6 },
        { symbol: 'AAPL', buysellindicator: 'S', quantity: 20, price: 57, packetSequence: 7 },
        { symbol: 'AMZN', buysellindicator: 'B', quantity: 25, price: 150, packetSequence: 8 },
        { symbol: 'AMZN', buysellindicator: 'S', quantity: 15, price: 155, packetSequence: 9 },
        { symbol: 'AMZN', buysellindicator: 'B', quantity: 20, price: 148, packetSequence: 10 },
        { symbol: 'META', buysellindicator: 'B', quantity: 10, price: 3000, packetSequence: 11 },
        { symbol: 'META', buysellindicator: 'B', quantity: 5, price: 2999, packetSequence: 12 },
        { symbol: 'META', buysellindicator: 'S', quantity: 15, price: 3020, packetSequence: 13 },
        { symbol: 'AMZN', buysellindicator: 'S', quantity: 10, price: 3015, packetSequence: 14 }
    ]
};

const PACKET_CONTENTS = [
    { name: 'symbol', type: 'ascii', size: 4 },
    { name: 'buysellindicator', type: 'ascii', size: 1 },
    { name: 'quantity', type: 'int32', size: 4 },
    { name: 'price', type: 'int32', size: 4 },
    { name: 'packetSequence', type: 'int32', size: 4 }
];

const PACKET_SIZE = PACKET_CONTENTS.reduce((acc, field) => acc + field.size, 0);

const createPayloadToSend = (data) => {
    let offset = 0;
    const buffer = Buffer.alloc(PACKET_SIZE);

    PACKET_CONTENTS.forEach((field) => {
        if (field.type === 'int32') buffer.writeInt32BE(data[field.name], offset);
        else if (field.type === 'ascii') buffer.write(data[field.name], offset, field.size, 'ascii');
        offset += field.size;
    });

    return buffer;
};

const orderBook = packetData.packetStream;
let BUFFER_COLLECTOR = Buffer.alloc(0);

const server = net.createServer((client) => {
    console.log('Client connected.');

    client.on('data', (data) => {
        BUFFER_COLLECTOR = Buffer.concat([BUFFER_COLLECTOR, data]);

        while (BUFFER_COLLECTOR.length >= 2) 
            {
            const header = BUFFER_COLLECTOR.slice(0, 2);
            const callType = header.readInt8(0);
            const resendSeq = header.readInt8(1);
            BUFFER_COLLECTOR = BUFFER_COLLECTOR.slice(2);

            if (callType === 1) 
            {
                orderBook.forEach((packet) => {
                    if (Math.random() > 0.75) return;
                    const payload = createPayloadToSend(packet);
                    client.write(payload);
                });
                client.end();
                console.log('Packets sent. Client disconnected.');
            }
            else if (callType === 2)
            {
                const packet = orderBook.find(p => p.packetSequence === resendSeq);
                if (packet) 
                {
                    const payload = createPayloadToSend(packet);
                    client.write(payload);
                    console.log('Packet resent.');
                }
            }
        }
    });

    client.on('end', () => {
        console.log('Client disconnected.');
    });
});

server.listen(3000, () => {
    console.log('TCP server started on port 3000.');
});
