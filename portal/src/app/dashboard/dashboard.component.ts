import { Component, OnInit } from '@angular/core';
import { EspService } from '../esp.service';
import { Title } from '@angular/platform-browser';

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.scss']
})
export class DashboardComponent implements OnInit {
  title = 'Dashboard';

  constructor(private espService: EspService) {}

  public setTitle( newTitle: string) {
  }

  ngOnInit(): void {
  }
}
