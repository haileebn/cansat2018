// const express = require('express');
const mongoClient = require('mongodb').MongoClient;
let SerialPort = require('serialport');

// const router = express();
const URL_CONNECT="mongodb://localhost:27017/cansat2017"
// const Readline = SerialPort.parsers.Readline;


// let serialPort = new SerialPort('COM5', {
//   baudRate: 9600
// });
let dem = 0;
let tong= 0;
let count=0;
// het gio lan 1. lan 2 ko cho cong tong nua.

let flag = 0;
// const port = 8080;
// const parser = serialPort.pipe(new Readline({ delimiter: '\r\n' }));
const start_time = new Date().getTime();

mongoClient.connect(URL_CONNECT, (err, db)=>{
	db.collection('AlFIMO1_4').find({}, { pres:1, tem:1, al: 1}).toArray((err, result)=>{
	// // res.setHeader('Content-Type', 'application/json');
		if (err) throw err;
		console.log(result);
		result.forEach(key => {
			console.log(key.pres, key.tem, key.al*100);
		});
		
	});
	// db.collection('allDevice').find().toArray((err, result)=>{
	// // // res.setHeader('Content-Type', 'application/json');
	// 	if (err) throw err;
	// 	let saiSoTuyetDoi = 0;
	// 	result.forEach((key, index) => {
	// 		saiSoTuyetDoi = saiSoTuyetDoi + key.al;
	// 		if(index === result.length - 1)
	// 			console.log((saiSoTuyetDoi/(result.length)));
	// 	});
		
	// });
			
});


// console.log(parser)

// router.get('/', (req, res) => {
//     res.setHeader("Access-Control-Allow-Origin", "*");
//     res.send("Hello!!!");
// });

// router.listen(port, () => {
//     console.log(`server:`);
// });

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
