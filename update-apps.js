#!/usr/bin/env node
'use strict'
if (!process.argv[7]) {
	console.error("usage: " + process.argv[1] + " <apps.json url> <news.json location> <new apps.json location> <safe apps.json location> <repository url> <safe repository url>");
	process.exit(1)
}
import fetch from "node-fetch"
import fs from "fs"
const news = JSON.parse(fs.readFileSync(process.argv[3]));
fetch(process.argv[2], { headers: { "user-agent": "AltStore/1 CFNetwork/1331.0.7 Darwin/21.4.0" }})
	.then(response => response.json())
	.then(appsjson => {
		appsjson.news = news
		const betaApps = []
		for (const app of appsjson.apps) {
			if (app.beta === true) {
				app.beta = false;
				app.name += " [Beta]"
				betaApps.push(app)
			}
		}
		appsjson.apps = betaApps
		appsjson.sourceURL = process.argv[6]
		appsjson.userInfo = {}
                appsjson.identifier = 'gq.nickchan.altstore-beta'
                appsjson.name = 'AltStore Beta'
		return appsjson
	}).then(newJson => {
		fs.writeFileSync(process.argv[4],JSON.stringify(newJson,null,2))
                newJson.name = 'AltStore Beta (Trusted bypass)'
		newJson.identifier = 'io.altstore.example' // pretend to be a hidden trusted source
		newJson.sourceURL = process.argv[7]
		fs.writeFileSync(process.argv[5],JSON.stringify(newJson,null,2))
	})
