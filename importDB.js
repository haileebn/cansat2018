// const express = require('express');
const mongoClient = require('mongodb').MongoClient;

// const URL_CONNECT="mongodb://qmt:fimo!54321@118.70.72.13:55555/qmt"
const URL_CONNECT="mongodb://localhost:27017/cansat2018"
const TestCollection = "com_md_50";

mongoClient.connect(URL_CONNECT, (err, db)=>{
	if (err) console.log(err);
	db.collection(TestCollection).find().toArray((err, result) => {
		// console.log(index);
		result.forEach((element, index) => {
			setTimeout(()=> {
				console.log("aaaaa" + result[index].time_import);
				db.collection("input").insert(element);
			}, result[index].time_import - result[0].time_import);
		});
	});
});



function checkZero(number){
    return number === 0 ? null : Number(number);
}
