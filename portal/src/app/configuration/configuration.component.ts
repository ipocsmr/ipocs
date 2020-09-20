import { Component, OnInit, ElementRef, Input } from '@angular/core';
import { EspService } from '../esp.service';
import { MatInput } from '@angular/material/input';
import { Mode } from '../mode.enum';

@Component({
  selector: 'app-configuration',
  templateUrl: './configuration.component.html',
  styleUrls: ['./configuration.component.scss']
})
export class ConfigurationComponent implements OnInit {
  public unitNameValue: string;
  public networkSsidValue: string;
  public networkPwdValue: string;
  public siteDataValue: string;
  public isESP8266:boolean = true;

  constructor(public espService: EspService) {
    this.espService.unitName$.subscribe(newVal => {
      this.unitNameValue = newVal;
    });
    this.espService.ssid$.subscribe(newVal => {
      this.networkSsidValue = newVal;
    });
    this.espService.pwd$.subscribe(newVal => {
      this.networkPwdValue = newVal;
    });
    this.espService.siteData$.subscribe(newVal => {
      this.siteDataValue = newVal;
    });
    this.espService.mode$.subscribe(value => {
      this.isESP8266 = value == Mode.esp8266;
    });
  }

  ngOnInit(): void {
  }

  saveUnitName(element: HTMLInputElement): void {
    this.espService.setUnitName(element.value);
  }

  saveSsid(element: HTMLInputElement): void {
    this.espService.setSsid(element.value);
  }

  savePwd(element: HTMLInputElement): void {
    this.espService.setPwd(element.value);
  }

  saveSiteData(element: HTMLInputElement): void {
    this.espService.setSiteData(element.value);
  }

  restartArduino(): void {
    this.espService.restartArduino();
  }

  restartEsp(): void {
    this.espService.restartEsp();
  }

  applyWiFi(): void {
    this.espService.applyWiFi();
  }

  resetEsp(): void {
    if (confirm("Are you sure to reset/erase ESP settings?")) {
      this.espService.resetEsp();
    }
  }
}
