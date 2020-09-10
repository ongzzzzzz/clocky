/* eslint-disable promise/no-nesting */
/* eslint-disable promise/always-return */
const functions = require('firebase-functions');
const admin = require('firebase-admin');
const fetch = require('node-fetch');

admin.initializeApp();
let rtdb = admin.database();
let date = new Date();

function checkStatus(res) {
    if (res.ok) { // res.status >= 200 && res.status < 300
        return res.json();
    } else {
        throw new Error(res.statusText);
    }
}

exports.updateData = functions.pubsub.schedule('every 1 minute').onRun((context) => {

    let dateRN = date.getFullYear() + "-" + (("0" + (date.getMonth() + 1)).slice(-2)) + "-" + (("0" + date.getDate()).slice(-2));
    let timeRN = date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds();

    try{
        rtdb.ref('calendar/accuracy').set({
            dateNow:dateRN,
            timeNow:timeRN,
        })
    } catch(e){ console.error(e); }

    // TODO: get weather from accuweather
    //TODO: test these functions, change back 1 hour after u test
    // TODO: get these data in clocky
    //TODO: update webapp  

    fetch('http://dataservice.accuweather.com/currentconditions/v1/234975?apikey=HuptOIEbq28g99YeSniD2juuFb8a2Btj')
        .then(checkStatus)
        .then(json => {
            console.log(JSON.stringify(json));
            if(json.length) return rtdb.ref('calendar/weather').set({
                EpochTime:json[0].EpochTime,
                HasPrecipitation:json[0].HasPrecipitation,
                IsDayTime:json[0].IsDayTime,
                PrecipitationType:json[0].PrecipitationType,
                Temperature:json[0].Temperature,
                WeatherText:json[0].WeatherText,
                
            }).then(_=>{console.log('Weather Updated');});
        })
        .catch(e=>{
            console.log(e)
        });
    
    return null;
})



// // Create and Deploy Your First Cloud Functions
// // https://firebase.google.com/docs/functions/write-firebase-functions
//
// exports.helloWorld = functions.https.onRequest((request, response) => {
//   functions.logger.info("Hello logs!", {structuredData: true});
//   response.send("Hello from Firebase!");
// });




//let date = new Date();

// // current date
// // adjust 0 before single digit date
// let date = ("0" + date.getDate()).slice(-2);

// // current month
// let month = ("0" + (date.getMonth() + 1)).slice(-2);

// // current year
// let year = date.getFullYear();

// // current hours
// let hours = date.getHours();

// // current minutes
// let minutes = date.getMinutes();

// // current seconds
// let seconds = date.getSeconds();

// // prints date in YYYY-MM-DD format
// console.log(year + "-" + month + "-" + date);

// // prints date & time in YYYY-MM-DD HH:MM:SS format
// console.log(year + "-" + month + "-" + date + " " + hours + ":" + minutes + ":" + seconds);

// // prints time in HH:MM format
// console.log(hours + ":" + minutes);
