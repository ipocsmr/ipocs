import { Injectable } from '@angular/core';
import { connectableObservableDescriptor } from 'rxjs/internal/observable/ConnectableObservable';
import { BehaviorSubject } from 'rxjs';
import { ArrayType } from '@angular/compiler';

export class Message {
  value: string;
  action: string;
}

export class File {
  name: string;
  size: number;
}

@Injectable({
  providedIn: 'root'
})
export class EspService {
  public unitId$ = new BehaviorSubject<number>(0);
  public ssid$ = new BehaviorSubject<string>("");
  public pwd$ = new BehaviorSubject<string>("");
  public siteData$ = new BehaviorSubject<string>("");
  public files$ = new BehaviorSubject<Array<File>>([]);
  public logs$ = new BehaviorSubject<Array<string>>([]);
  public versionArduino$ = new BehaviorSubject<string>("");
  public versionEsp$ = new BehaviorSubject<string>("");

  private webSocket: WebSocket;
  private connectTimer: number;
  private pinger: number;

  constructor() {
    this.connect();
  }

  private connect(): void {
    this.webSocket = new WebSocket("ws://" + window.location.hostname + ":81");
    this.webSocket.onopen = (event) => this.onOpen(event);
    this.webSocket.onclose = (event) => this.onClose(event);
    this.webSocket.onmessage = (message) => this.onMessage(message);
  }

  private onOpen(event: Event): void {
    console.log(event);
    clearTimeout(this.connectTimer);
    this.pinger = setInterval(() => {
      this.sendMessage({
        action: "ping",
        value: null
      });
    }, 500);
  }

  private onClose(event: Event): void {
    console.log(event);
    clearTimeout(this.connectTimer);
    clearInterval(this.pinger);
    this.connectTimer = setTimeout(() => {
      this.connect();
    }, 2000);
  }

  private onMessage(message: MessageEvent) {
    let data: Message = JSON.parse(message.data);
    console.log(data);
    switch (data.action) {
      case "valueUnitId":
        this.unitId$.next(parseInt(data.value));
        break;
      case "valueSsid":
        this.ssid$.next(data.value);
        break;
      case "valuePwd":
        this.pwd$.next(data.value);
        break;
      case "valueSiteData":
        this.siteData$.next(data.value);
        break;
      case "valueFiles":
        this.files$.next(JSON.parse(data.value));
        break;
      case "versionArduino":
        this.versionArduino$.next(data.value);
        break;
      case "versionEsp":
        this.versionEsp$.next(data.value);
        break;
      case "log":
        this.logs$.next(this.logs$.value.concat([data.value]));
        break;
    }
  }
  private sendMessage(message: Message) {
    this.webSocket.send(JSON.stringify(message));
  }

  public setUnitId(unitId: number): void {
    let data: Message = {
      action: "setUnitId",
      value: unitId.toString()
    }
    this.sendMessage(data);
  }

  public setSsid(ssid: string): void {
    this.sendMessage({
      action: "setSsid",
      value: ssid
    });
  }

  public setPwd(ssid: string): void {
    this.sendMessage({
      action: "setSsid",
      value: ssid
    });
  }

  public setSiteData(ssid: string): void {
    this.sendMessage({
      action: "setSsid",
      value: ssid
    });
  }

  public deleteFile(fileName: string): void {
    this.sendMessage({
      action: "deleteFile",
      value: fileName
    });
  }

  public verifyFile(fileName: string): void {
    this.sendMessage({
      action: "verifyFile",
      value: fileName
    });
  }

  public verifyFlash(fileName: string): void {
    this.sendMessage({
      action: "verifyFlash",
      value: fileName
    });
  }

  public programFlash(fileName: string): void {
    this.sendMessage({
      action: "programFlash",
      value: fileName
    });
  }

  public restartArduino(fileName: string): void {
    this.sendMessage({
      action: "restartArduino",
      value: null
    });
  }

  public restartEsp(fileName: string): void {
    this.sendMessage({
      action: "restartEsp",
      value: null
    });
  }

  public resetEsp(fileName: string): void {
    this.sendMessage({
      action: "resetEsp",
      value: null
    });
  }
}