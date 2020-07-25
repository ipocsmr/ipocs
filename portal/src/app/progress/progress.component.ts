import { Component, OnInit } from '@angular/core';
import { BehaviorSubject } from 'rxjs';

@Component({
  selector: 'app-progress',
  templateUrl: './progress.component.html',
  styleUrls: ['./progress.component.scss']
})
export class ProgressComponent implements OnInit {
  public progress$ = new  BehaviorSubject<number>(0);
  public message$ = new BehaviorSubject<string>("");

  constructor() { }

  ngOnInit(): void {
  }
}
