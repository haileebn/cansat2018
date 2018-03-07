// const express = require('express');
const mongoClient = require('mongodb').MongoClient;
let SerialPort = require('serialport');

// const router = express();
// const URL_CONNECT="mongodb://localhost:3001/meteor"
const URL_CONNECT="mongodb://localhost:27017/final"
const Readline = SerialPort.parsers.Readline;


let serialPort = new SerialPort('COM3', {
  baudRate: 9600
});
let dem = 0;
let tong= 0;
let count=0;
// het gio lan 1. lan 2 ko cho cong tong nua.

let flag = 0;
// const port = 8080;
const parser = serialPort.pipe(new Readline({ delimiter: '\r\n' }));
const start_time = new Date().getTime();

const timeEnd = 5*60*1000;
const id = 34;
const d = 400;
const Thong_tin_Str = `test truyen thong final`;
const TestCollection = `input 12`;


// const id = 2;
// const Thong_tin_Str = "SVĐ mỹ đình 400m";
// const TestCollection = "SVD400m";

// const id = 3;
// const Thong_tin_Str = "SVĐ mỹ đình 600m";
// const TestCollection = "SVD600m";

// const id = 4;
// const Thong_tin_Str = "SVĐ mỹ đình 800m";
// const TestCollection = "SVD800m";

// const id = 5;
// const Thong_tin_Str = "SVĐ mỹ đình 1000m";
// const TestCollection = "SVD1000m";

// const id = 6;
// const Thong_tin_Str = "SVĐ mỹ đình 1200m";
// const TestCollection = "SVD1200m";



mongoClient.connect(URL_CONNECT, (err, db)=>{
	if (err) console.log(err);
	parser.on('data', (result) => {
		let json = JSON.parse(result);
		let values = json["values"];
		let data = {
		    'time_read': Number(json.time),
		    'time_import': new Date().getTime(),
		    'al': Number(values.al)/100,
		    'lat': checkZero(values.LAT),
		    'lon': checkZero(values.LON),
		    'tem': checkZero(values.temp)/10,
		    'hum': checkZero(values.hud)/10,
		    'pres': checkZero(values.pressure),
		    'pm1': checkZero(values.pm1),
		    'pm25': checkZero(values.pm25),
		    'pm10': checkZero(values.pm10),
			'co': checkZero(values.CO),
			'dem': Number(values.dem)
		    }
		dem++;
		// console.log(data);
		// if(data.dem === 9){
		// 	tong += (data.dem - dem + 1);
		// 	dem = 0;
		// }

		// console.log("data.dem "+ data.dem, "dem" + dem, "tong" + tong);
		if(data.time_import - start_time > timeEnd){
			flag++;
			if(flag === 1){
				console.log("het gioooooooooo");
				tong += (data.dem - dem + 1);
				db.collection('thongTin').update({ "id": id}, {
					"so tin da gui": data.dem,
					"so tin da nhan": dem -1,
					"So tin bi mat": tong, "id": id,
					"lastTime": new Date().getTime(),
					"thongtin": Thong_tin_Str
				},{ upsert: true });
			}else {
				console.log("het gioooooooooo lan "+ flag);
				console.log(`so ban tin bi mat: ${tong}`, `tong so tin da gui: ${data.dem + 1}`);
			}
		}else {
			db.collection(TestCollection).insertOne(data, (err, result)=>{
				console.log(`So tin da truyen ${data.dem + 1}`, "So tin da nhan" + dem);
		});
			// db.collection('thongTin').update({ "id": 10}, { "soBanTinBiMat": tong, "id": 10, "lastTime": new Date().getTime(), "thongtin": "Đo CO 408 trực tiếp trên laptop"}, { upsert: true });
		}
		
	});
});


function checkZero(number){
    return number === 0 ? null : Number(number);
}

/**
 * t,p la nhiet do, ap suat o tren cao
 * t0, p0 la nhiet do, ap suat ban dau duoi mat dat
 */
// function getAltitudePressure(t, p, t0, p0){
//     const R = 287.05; // J/Kg°K
//     const g = 9.80665; //m/s 2.

//     if(t === 0 || t0 === 0){
//         return 0;
//     }
//     else {
//         const k = 273.15;
//         const T = t + k;
//         const T0 = t0 + k;
//         if(p === 0) return 0;
//         else {
//             return ((R/g)*((T+ T0)/2)*Math.log10(p0/p));
//         }
//     }
// }
