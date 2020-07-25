import { Component, OnInit } from '@angular/core';
import { Observable } from 'rxjs';
import { EspService } from '../esp.service';

@Component({
  selector: 'app-logging',
  templateUrl: './logging.component.html',
  styleUrls: ['./logging.component.scss']
})
export class LoggingComponent implements OnInit {
  displayedColumns = ["message"];
  logs$: Observable<string[]>;

  constructor(private espService: EspService) {
    this.logs$ = espService.logs$.asObservable();
  }

  ngOnInit(): void {
  }

  clearLogs(): void {
    this.espService.logs$.next([]);
  }

}
