const AWS = require('aws-sdk');
const dynamoDB = new AWS.DynamoDB.DocumentClient();

exports.handler = async (event) => {
    // Asume que el payload viene en el cuerpo del evento
    const payload = JSON.parse(event.body); 
    
    const params = {
        TableName: 'DH11Data',
        Item: {
            device_id: payload.end_device_ids.device_id,
            timestamp: payload.received_at,
            temperature: payload.uplink_message.decoded_payload.temperature,
            humidity: payload.uplink_message.decoded_payload.humidity
        }
    };
    
    try {
        await dynamoDB.put(params).promise();
        return { statusCode: 200, body: 'Data stored successfully' };
    } catch (error) {
        console.error('Error storing data: ', error);
        return { statusCode: 500, body: 'Error storing data' };
    }
    //Esto es un comentario
};
