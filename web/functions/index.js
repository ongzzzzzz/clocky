/* eslint-disable promise/no-nesting */
/* eslint-disable promise/always-return */
const functions = require('firebase-functions');
const admin = require('firebase-admin');
const fetch = require('node-fetch');

admin.initializeApp();
let rtdb = admin.database();
let date = new Date().toLocaleString('en-US', { timeZone: 'Asia/Kuala_Lumpur' });

console.log(`Malaysia time now is ${date}`);
// TODO: get weather from accuweather
//TODO: test these functions, change back 1 hour after u test
// TODO: get these data in clocky
//TODO: update webapp  

function checkStatus(res) {
    if (res.ok) { // res.status >= 200 && res.status < 300
        return res.json();
    } else {
        throw new Error(res.statusText);
    }
}

exports.updateData = functions.pubsub.schedule('every 5 minutes').onRun((context) => {

    let minutes = date.getMinutes() < 10 ? "0"+date.getMinutes() : date.getMinutes();
    let seconds = date.getSeconds() < 10 ? "0"+date.getSeconds() : date.getSeconds();

    let dateRN = date.getFullYear() + "-" + (("0" + (date.getMonth() + 1)).slice(-2)) + "-" + (("0" + date.getDate()).slice(-2));
    let timeRN = date.getHours() + ":" + minutes + ":" + seconds;

    let accurator = {}; accurator["dateNow"] = dateRN; accurator["timeNow"] = timeRN;

    rtdb.ref('calendar/accuracy').set(accurator).catch(e => console.log(e));
    

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
            return null;
        })
        .catch(e => {
            console.log(e);
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