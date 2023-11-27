const AWS = require('aws-sdk');
const moment = require('moment-timezone');
AWS.config.update({ region: 'us-east-1' });
const TimestreamWrite = new AWS.TimestreamWrite();

exports.handler = async (event) => {
    try {
        // Extrae los datos del evento
        const { carbono, humedad, nitrogeno, ozono, pm10, pm25, temperatura, tiempo, deviceId } = event;
        console.log("Datos del evento:", event); // Agregar para depurar


        // Convierte la marca de tiempo a la zona horaria de Paraguay
        let timestampParaguay = moment(tiempo).tz('America/Asuncion');
        let fechaParaguaya = timestampParaguay.format('DD/MM/YYYY HH:mm'); // Extrae la fecha y hora
        console.log("FECHA Y HORA PARAGUAYA:", fechaParaguaya);
      
        // Prepara los datos para Timestream
        let records = prepareDataForTimestream({ carbono, humedad, nitrogeno, ozono, pm10, pm25, temperatura }, timestampParaguay, deviceId, fechaParaguaya);
        console.log("Registros preparados para Timestream:", records);
        // Envía los datos a Timestream
        await writeToTimestream(records);

        return { status: 'Procesado con éxito' };
    } catch (error) {
        console.error('Error en la función Lambda:', error);
        throw error;
    }
};

function prepareDataForTimestream(sensorData, timestampParaguay, deviceId, fechaParaguaya) {
    let records = [];

    for (const key in sensorData) {
        if (sensorData.hasOwnProperty(key) && sensorData[key] !== undefined) {
            records.push({
                Dimensions: [
                    {
                        Name: 'DeviceId',
                        Value: deviceId
                    },
                    {
                        Name: 'fechaParaguaya',
                        Value: fechaParaguaya
                    }
                ],
                MeasureName: key,
                MeasureValue: sensorData[key].toString(),
                MeasureValueType: 'DOUBLE',
                Time: moment(timestampParaguay).valueOf().toString(),
                TimeUnit: 'MILLISECONDS'
            });
        }
    }

    return records;
}



async function writeToTimestream(records) {
    const params = {
        DatabaseName: 'CalidadAire',
        TableName: 'Lago',
        Records: records
    };

    try {
        await TimestreamWrite.writeRecords(params).promise();
        console.log('Datos escritos en Timestream con éxito');
    } catch (error) {
        console.error('Error al escribir en Timestream:', error);
        throw error;
    }
}
