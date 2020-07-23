import { Component, OnInit } from '@angular/core';
import { EspService } from '../esp.service';

@Component({
  selector: 'app-flash-esp',
  templateUrl: './flash-esp.component.html',
  styleUrls: ['./flash-esp.component.scss']
})
export class FlashEspComponent implements OnInit {

  constructor(public espService: EspService) { }

  ngOnInit(): void {
  }

}
