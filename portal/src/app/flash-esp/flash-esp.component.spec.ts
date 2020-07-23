import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { FlashEspComponent } from './flash-esp.component';

describe('FlashEspComponent', () => {
  let component: FlashEspComponent;
  let fixture: ComponentFixture<FlashEspComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ FlashEspComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(FlashEspComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
