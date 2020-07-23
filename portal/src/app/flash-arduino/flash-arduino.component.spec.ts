import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { FlashArduinoComponent } from './flash-arduino.component';

describe('FlashArduinoComponent', () => {
  let component: FlashArduinoComponent;
  let fixture: ComponentFixture<FlashArduinoComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ FlashArduinoComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(FlashArduinoComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
