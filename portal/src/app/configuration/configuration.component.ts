import { Component, OnInit, ElementRef, Input } from '@angular/core';
import { EspService } from '../esp.service';
import { MatInput } from '@angular/material/input';

@Component({
  selector: 'app-configuration',
  templateUrl: './configuration.component.html',
  styleUrls: ['./configuration.component.scss']
})
export class ConfigurationComponent implements OnInit {
  public unitIdValue: number;
  public networkSsidValue: string;
  public networkPwdValue: string;
  public siteDataValue: string;

  constructor(public espService: EspService) {
    this.espService.unitId$.subscribe(newVal => {
      this.unitIdValue = newVal;
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
  }

  ngOnInit(): void {
  }

  saveUnitId(element: HTMLInputElement): void {
    this.espService.setUnitId(parseInt(element.value));
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
}
